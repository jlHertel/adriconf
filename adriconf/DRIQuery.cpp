#include <giomm.h>
#include <xf86drm.h>
#include <iomanip>
#include <fcntl.h>
#include <glibmm/i18n.h>
#include <string>

#include "DRIQuery.h"
#include "PCIDatabaseQuery.h"


DRIQuery::DRIQuery(LoggerInterface *logger) : logger(logger) {

}


bool DRIQuery::isSystemSupported() {
    if (std::string(std::getenv("XDG_SESSION_TYPE")) == "wayland") {
#ifdef ENABLE_XWAYLAND
        HelpersWayland hw;
        return hw.hasProperLibEGL();
#endif
    } else if (std::string(std::getenv("XDG_SESSION_TYPE")) == "x11") {
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

    return false;
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

        if (std::string(std::getenv("XDG_SESSION_TYPE")) == "x11") {
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

            auto parsedSections = Parser::parseAvailableConfiguration(options, locale);
            config.setSections(parsedSections);

            configurations.emplace_back(config);
        } else if (std::string(std::getenv("XDG_SESSION_TYPE")) == "wayland") {
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

            auto parsedSections = Parser::parseAvailableConfiguration(options, locale);
            config.setSections(parsedSections);

            configurations.emplace_back(config);
#endif //ENABLE_XWAYLAND
        }


    }

    XCloseDisplay(display);

    return configurations;
}

std::map<Glib::ustring, GPUInfo_ptr> DRIQuery::enumerateDRIDevices(const Glib::ustring &locale) {
    std::map<Glib::ustring, GPUInfo_ptr> gpus;

    PCIDatabaseQuery pciQuery;

    drmDevicePtr enumeratedDevices[MESA_MAX_DRM_DEVICES];
    int deviceCount = drmGetDevices2(0, enumeratedDevices, MESA_MAX_DRM_DEVICES);

    for (int i = 0; i < deviceCount; i++) {
        GPUInfo_ptr gpu = std::make_shared<GPUInfo>();

        gpu->setPciId(Glib::ustring::compose(
                "pci-%1_%2_%3_%4",
                Glib::ustring::format(std::setfill(L'0'), std::setw(4), std::hex,
                                      enumeratedDevices[i]->businfo.pci->domain),
                Glib::ustring::format(std::setfill(L'0'), std::setw(2), std::hex,
                                      enumeratedDevices[i]->businfo.pci->bus),
                Glib::ustring::format(std::setfill(L'0'), std::setw(2), std::hex,
                                      enumeratedDevices[i]->businfo.pci->dev),
                enumeratedDevices[i]->businfo.pci->func
        ));

        int fd = open(enumeratedDevices[i]->nodes[DRM_NODE_RENDER], O_RDONLY);
        drmVersionPtr versionPtr = drmGetVersion(fd);

        gpu->setDriverName(versionPtr->name);
        /* LibDRM returns the kernel-level driver name
         * This is wrong for AMD cards, because at kernel it's called amdgpu/radeon and at mesa-level it is radeonsi
         * This is a small fix to try to solve this issue
         */
        if (gpu->getDriverName() == "amdgpu" || gpu->getDriverName() == "radeon") {
            gpu->setDriverName("radeonsi");
        }

        drmFreeVersion(versionPtr);
        close(fd);

        gpu->setVendorId(enumeratedDevices[i]->deviceinfo.pci->vendor_id);
        gpu->setDeviceId(enumeratedDevices[i]->deviceinfo.pci->device_id);

        gpu->setVendorName(pciQuery.queryVendorName(gpu->getVendorId()));
        gpu->setDeviceName(pciQuery.queryDeviceName(gpu->getVendorId(), gpu->getDeviceId()));

        const char *driverOptions;

        if (std::string(std::getenv("XDG_SESSION_TYPE")) == "x11") {
            // Load the driver supported options
            driverOptions = this->queryDriverConfig((const char *) gpu->getDriverName().c_str());
        } else if (std::string(std::getenv("XDG_SESSION_TYPE")) == "wayland") {
#ifdef ENABLE_XWAYLAND
            HelpersWayland hw;
            driverOptions = hw.queryDriverConfig();
#endif //ENABLE_XWAYLAND
        }

        // If for some reason mesa is unable to query the options we simply skip this gpu
        if (driverOptions == nullptr) {
            this->logger->error(Glib::ustring::compose(
                    _("Unable to extract configuration for driver %1"), gpu->getDriverName()
            ));

            continue;
        }

        Glib::ustring options(driverOptions);
        if (options.empty()) {
            this->logger->error(Glib::ustring::compose(
                    _("Unable to extract configuration for driver %1"), gpu->getDriverName()
            ));

            continue;
        }

        auto parsedSections = Parser::parseAvailableConfiguration(options, locale);
        gpu->setSections(parsedSections);

        gpus[gpu->getPciId()] = gpu;
    }

    drmFreeDevices(enumeratedDevices, deviceCount);

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
