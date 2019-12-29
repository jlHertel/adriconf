#include <xf86drm.h>
#include <iomanip>
#include <glibmm/i18n.h>
#include <string>

#include <stdexcept>

#include "DRIQuery.h"
#include "PCIDatabaseQuery.h"

#include <EGL/egl.h>

DRIQuery::DRIQuery(
        LoggerInterface *logger,
        ParserInterface *parser,
        PCIDatabaseQueryInterface *pciQuery,
        DRMDeviceFactoryInterface *drmDeviceFactory,
        GBMDeviceFactoryInterface *gbmDeviceFactory,
        EGLDisplayFactoryInterface *eglWrapper,
        bool isWaylandSession
) : logger(logger),
    parser(parser),
    pciQuery(pciQuery),
    drmDeviceFactory(drmDeviceFactory),
    gbmDeviceFactory(gbmDeviceFactory),
    eglDisplayFactory(eglWrapper),
    isWaylandSession(isWaylandSession) {}

bool DRIQuery::isSystemSupported() {
    if (this->isWaylandSession) {
#ifdef ENABLE_XWAYLAND
        HelpersWayland hw;
        return hw.hasProperLibEGL();
#endif
    }

    this->getScreenDriver = (glXGetScreenDriver_t *) glXGetProcAddress((const GLubyte *) "glXGetScreenDriver");
    this->getDriverConfig = (glXGetDriverConfig_t *) glXGetProcAddress((const GLubyte *) "glXGetDriverConfig");
    this->getGlxExtensionsString = (glXQueryExtensionsString_t *) glXGetProcAddress(
            (const GLubyte *) "glXQueryExtensionsString");

    if (!this->getScreenDriver || !this->getDriverConfig || !this->getGlxExtensionsString) {
        this->logger->error(_("Error getting function pointers. LibGL must be too old."));
        return false;
    }

    return true;
}

const char *DRIQuery::queryDriverName(int s) {
    Display *display;
    const char *ret;

    if (!(display = XOpenDisplay(nullptr))) {
        this->logger->error(_("Couldn't open X display"));
        return "";
    }

    this->getScreenDriver = (glXGetScreenDriver_t *) glXGetProcAddress((const GLubyte *) "glXGetScreenDriver");
    ret = (*(this->getScreenDriver))(display, s);

    XCloseDisplay(display);

    return ret;
}

const char *DRIQuery::queryDriverConfig(const char *dn) {
    Display *display;
    const char *ret;

    if (!(display = XOpenDisplay(nullptr))) {
        this->logger->error(_("Couldn't open X display"));
        return "";
    }

    this->getDriverConfig = (glXGetDriverConfig_t *) glXGetProcAddress((const GLubyte *) "glXGetDriverConfig");
    ret = (*(this->getDriverConfig))(dn);

    XCloseDisplay(display);

    return ret;
}

std::list<DriverConfiguration> DRIQuery::queryDriverConfigurationOptions(const Glib::ustring &locale) {
    this->logger->debug(_("Querying driver configuration options"));
    std::list<DriverConfiguration> configurations;
    Display *display;

    if (!(display = XOpenDisplay(nullptr))) {
        this->logger->error(_("Couldn't open X display"));
        return configurations;
    }

    int screenCount = ScreenCount (display);

    for (int i = 0; i < screenCount; i++) {
        DriverConfiguration config;
        config.setScreen(i);

        if (!this->isWaylandSession) {
            /* Call glXGetClientString to load vendor libs on glvnd enabled systems */
            glXGetClientString(display, GLX_EXTENSIONS);

            auto driverName = this->queryDriverName(i);
            if (driverName == nullptr) {
                this->logger->error(Glib::ustring::compose(
                        _("Unable to extract driver name for screen %1"), i
                ));
                continue;
            }
            config.setDriverName(driverName);

            auto driverOptions = this->queryDriverConfig(driverName);
            // If for some reason mesa is unable to query the options we simply skip this gpu
            if (driverOptions == nullptr) {
                this->logger->error(Glib::ustring::compose(
                        _("Unable to extract configuration for driver %1"), config.getDriverName()
                ));

                continue;
            }

            Glib::ustring options(driverOptions);
            if (options.empty()) {
                this->logger->error(Glib::ustring::compose(
                        _("Unable to extract configuration for driver %1"), config.getDriverName()
                ));

                continue;
            }

            auto parsedSections = this->parser->parseAvailableConfiguration(options, locale);
            config.setSections(parsedSections);

            configurations.emplace_back(config);
        } else {
#ifdef ENABLE_XWAYLAND
            HelpersWayland hw;
            auto driverName = hw.queryDriverName();
            auto driverOptions = hw.queryDriverConfig();

            config.setDriverName(driverName);
            // If for some reason mesa is unable to query the options we simply skip this gpu
            if (driverOptions == nullptr) {
                this->logger->error(Glib::ustring::compose(
                        _("Unable to extract configuration for driver %1"), config.getDriverName()
                ));

                continue;
            }

            Glib::ustring options(driverOptions);
            if (options.empty()) {
                this->logger->error(Glib::ustring::compose(
                        _("Unable to extract configuration for driver %1"), config.getDriverName()
                ));

                continue;
            }

            auto parsedSections = this->parser->parseAvailableConfiguration(options, locale);
            config.setSections(parsedSections);

            configurations.emplace_back(config);
#endif //ENABLE_XWAYLAND
        }
    }

    XCloseDisplay(display);

    return configurations;
}

std::map<Glib::ustring, GPUInfo_ptr> DRIQuery::enumerateDRIDevices(const Glib::ustring &locale) {
    this->logger->debug(_("Enumerating DRI Devices"));
    std::map<Glib::ustring, GPUInfo_ptr> gpus;

    auto drmDevices = this->drmDeviceFactory->getDevices();
    this->logger->debug(Glib::ustring::compose(_("Found %1 devices"), drmDevices.size()));

    int i = 0;
    for (auto &device : drmDevices) {
        i++;
        try {
            GPUInfo_ptr gpu = std::make_shared<GPUInfo>();

            gpu->setPciId(device->getFormattedPCIId());
            this->logger->debug(Glib::ustring::compose(_("Processing GPU with PCI ID \"%1\""), gpu->getPciId()));
            gpu->setVendorId(device->getVendorPCIId());
            gpu->setDeviceId(device->getDevicePCIId());

            gpu->setVendorName(this->pciQuery->queryVendorName(gpu->getVendorId()));
            gpu->setDeviceName(this->pciQuery->queryDeviceName(gpu->getVendorId(), gpu->getDeviceId()));

            this->logger->debug(
                    Glib::ustring::compose(
                            _("GPU has been detected as \"%1\" from \"%2\""),
                            gpu->getDeviceName(),
                            gpu->getVendorName()
                    )
            );


            this->logger->debug(
                    Glib::ustring::compose(
                            _("Generating gbm device for path \"%1\""),
                            device->getDeviceRenderNodeName()
                    )
            );

            GBMDevice gbmDevice = this->gbmDeviceFactory->generateDeviceFromPath(device->getDeviceRenderNodeName());

            this->logger->debug(
                    Glib::ustring::compose(
                            _("Generating EGL Display from GBM device for \"%1\""),
                            device->getDeviceRenderNodeName()
                    )
            );

            auto display = this->eglDisplayFactory->createDisplayFromGBM(gbmDevice);
            gpu->setDriverName(display->getDriverName());
            this->logger->debug(Glib::ustring::compose(_("GPU \"%1\" has the driver \"%2\""), i, gpu->getDriverName()));

            this->logger->debug(_("Loading driver options"));
            const char *driverOptions = display->getDriverOptions();

            // If for some reason mesa is unable to query the options we simply skip this gpu
            if (driverOptions == nullptr) {
                this->logger->error(
                        Glib::ustring::compose(
                                _("Unable to extract configuration for driver %1"),
                                gpu->getDriverName()
                        )
                );

                continue;
            }

            Glib::ustring options(driverOptions);
            if (options.empty()) {
                this->logger->error(
                        Glib::ustring::compose(
                                _("Unable to extract configuration for driver %1"),
                                gpu->getDriverName()
                        )
                );

                continue;
            }

            auto parsedSections = this->parser->parseAvailableConfiguration(options, locale);
            gpu->setSections(parsedSections);

            gpus[gpu->getPciId()] = gpu;
        } catch (std::runtime_error &ex) {
            this->logger->error(Glib::ustring::compose(_("Skipping device %1 due to error: %2"), i, ex.what()));
        }
    }

    return gpus;
}

bool DRIQuery::canHandle() {
    Display *display;

    if (!(display = XOpenDisplay(nullptr))) {
        this->logger->error(_("Couldn't open X display"));
        return false;
    }

    int screenCount = ScreenCount (display);

    //Should handle multiple screen case
    for (int i = 0; i < screenCount; i++) {
        DriverConfiguration config;
        config.setScreen(i);

        /* Check if driver has mesa query extension or not? */
        const char *extensionString;
        extensionString = (*(this->getGlxExtensionsString))(display, i);
        std::string possibleExts(extensionString);
        if (possibleExts.find("GLX_MESA_query_renderer") == std::string::npos ||
            this->getScreenDriver == nullptr ||
            this->getDriverConfig == nullptr) {
            this->logger->error(_("Closed source driver!!"));
            return false;
        }
    }

    return true;
}
