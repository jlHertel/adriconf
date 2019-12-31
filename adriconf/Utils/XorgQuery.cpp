#include "XorgQuery.h"

XorgQuery::XorgQuery(LoggerInterface *logger, TranslatorInterface *translator, ParserInterface *parser) :
        logger(logger), translator(translator), parser(parser) {
    getScreenDriverName = (const char *(*)(Display *, int)) glXGetProcAddress((const GLubyte *) "glXGetScreenDriver");
    getDriverOptions = (const char *(*)(const char *)) glXGetProcAddress((const GLubyte *) "glXGetDriverConfig");
}

std::list<DriverConfiguration> XorgQuery::queryDriverConfigurationOptions(const Glib::ustring &locale) {
    std::list<DriverConfiguration> configs;

    Display *display;

    if (!(display = XOpenDisplay(nullptr))) {
        this->logger->error(this->translator->trns("Couldn't open X display"));
        return configs;
    }

    int screenCount = ScreenCount (display);

    for (int i = 0; i < screenCount; i++) {
        DriverConfiguration config;
        config.setScreen(i);

        /* Call glXGetClientString to load vendor libs on glvnd enabled systems */
        glXGetClientString(display, GLX_EXTENSIONS);

        auto driverName = this->getScreenDriverName(display, i);
        if (driverName == nullptr) {
            this->logger->error(Glib::ustring::compose(
                    this->translator->trns("Unable to extract driver name for screen %1"), i
            ));
            continue;
        }
        config.setDriverName(driverName);

        auto driverOptions = this->getDriverOptions(driverName);
        // If for some reason mesa is unable to query the options we simply skip this gpu
        if (driverOptions == nullptr) {
            this->logger->error(
                    Glib::ustring::compose(
                            this->translator->trns("Unable to extract configuration for driver %1"),
                            config.getDriverName()
                    )
            );

            continue;
        }

        Glib::ustring options(driverOptions);
        if (options.empty()) {
            this->logger->error(
                    Glib::ustring::compose(
                            this->translator->trns("Unable to extract configuration for driver %1"),
                            config.getDriverName()
                    )
            );

            continue;
        }

        auto parsedSections = this->parser->parseAvailableConfiguration(options, locale);
        config.setSections(parsedSections);

        configs.emplace_back(config);
    }

    return configs;
}

bool XorgQuery::checkNecessaryExtensions() {
    /* There is no reliable way to check if X11 supports the necessary extensions
     * so we just check if this is a open-source driver
     */
    Display *display;

    if (!(display = XOpenDisplay(nullptr))) {
        this->logger->error(this->translator->trns("Couldn't open X display"));
        return false;
    }
    const char *glXextensions = glXGetClientString(display, GLX_EXTENSIONS);
    std::string possibleExts(glXextensions);
    if (possibleExts.find("GLX_MESA_query_renderer") == std::string::npos) {
        this->logger->error(
                this->translator->trns(
                        "Missing GLX_MESA_query_renderer extension. Probably this is a closed-source driver."
                )
        );
        XCloseDisplay(display);
        return false;
    }

    XCloseDisplay(display);

    return true;
}