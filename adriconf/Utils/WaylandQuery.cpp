#include "WaylandQuery.h"

WaylandQuery::WaylandQuery(
        LoggerInterface *logger,
        TranslatorInterface *translator,
        ParserInterface *parser,
        EGLDisplayFactoryInterface *eglDisplayFactory
) : logger(logger), translator(translator), parser(parser), eglDisplayFactory(eglDisplayFactory) {}

std::list<DriverConfiguration> WaylandQuery::queryDriverConfigurationOptions(const Glib::ustring &locale) {
    std::list<DriverConfiguration> configs;

    // Wayland doesn't has the concept of screens, so we have a single configuration object
    DriverConfiguration config;
    config.setScreen(0);

    auto display = this->eglDisplayFactory->createDefaultDisplay();

    config.setDriverName(display->getDriverName());
    const char *driverOptions = display->getDriverOptions();
    if (driverOptions == nullptr) {
        this->logger->error(
                Glib::ustring::compose(
                        this->translator->trns("Unable to extract configuration for driver %1"),
                        config.getDriverName()
                )
        );

        return configs;
    }

    Glib::ustring options(driverOptions);
    if (options.empty()) {
        this->logger->error(
                Glib::ustring::compose(
                        this->translator->trns("Unable to extract configuration for driver %1"),
                        config.getDriverName()
                )
        );

        return configs;
    }

    auto parsedSections = this->parser->parseAvailableConfiguration(options, locale);
    config.setSections(parsedSections);
    configs.emplace_back(config);

    return configs;
}

bool WaylandQuery::checkNecessaryExtensions() {
    auto display = this->eglDisplayFactory->createDefaultDisplay();

    auto extensions = display->getExtensions();
    if (extensions.find("EGL_MESA_query_driver") == std::string::npos) {
        this->logger->error(
                this->translator->trns("Driver doesn't support extension \"EGL_MESA_query_driver\"")
        );
    }

    return true;
}
