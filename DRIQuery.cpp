#include <giomm.h>
#include <xf86drm.h>
#include <iomanip>
#include <fcntl.h>
#include <glibmm/i18n.h>

#include "DRIQuery.h"
#include "PCIDatabaseQuery.h"


DRIQuery::DRIQuery() {
    this->getScreenDriver = (glXGetScreenDriver_t *) glXGetProcAddress((const GLubyte *) "glXGetScreenDriver");
    this->getDriverConfig = (glXGetDriverConfig_t *) glXGetProcAddress((const GLubyte *) "glXGetDriverConfig");
    this->getRendererInfo = (glXQueryRenderer_t *) glXGetProcAddress((const GLubyte *) "glXQueryRendererIntegerMESA");

    if (!this->getScreenDriver || !this->getDriverConfig || !this->getRendererInfo) {
        std::cerr << _("Error getting function pointers. LibGL must be too old.") << std::endl;
    }
}

std::list<DriverConfiguration> DRIQuery::queryDriverConfigurationOptions(const Glib::ustring &locale) {
    std::list<DriverConfiguration> configurations;

    Display *display;

    if (!(display = XOpenDisplay(nullptr))) {
        std::cerr << _("Couldn't open X display") << std::endl;
        return configurations;
    }

    /* Call glXGetClientString to load vendor libs on glvnd enabled systems */
    glXGetClientString(display, GLX_EXTENSIONS);

    int screenCount = ScreenCount (display);

    for (int i = 0; i < screenCount; i++) {
        DriverConfiguration config;
        config.setScreen(i);

        unsigned int pciID = 0;
        (*(this->getRendererInfo))(display, i, 0, GLX_RENDERER_VENDOR_ID_MESA, &pciID);
        config.setVendorId(static_cast<uint16_t>(pciID));

        (*(this->getRendererInfo))(display, i, 0, GLX_RENDERER_DEVICE_ID_MESA, &pciID);
        config.setDeviceId(static_cast<uint16_t>(pciID));

        auto driverName = (*(this->getScreenDriver))(display, i);
        config.setDriverName(driverName);

        auto driverOptions = (*(this->getDriverConfig))(driverName);
        Glib::ustring options(driverOptions);

        auto parsedSections = Parser::parseAvailableConfiguration(options, locale);
        config.setSections(parsedSections);

        configurations.emplace_back(config);
    }

    XCloseDisplay(display);

    return std::move(configurations);
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

        drmFreeVersion(versionPtr);
        close(fd);

        gpu->setVendorId(enumeratedDevices[i]->deviceinfo.pci->vendor_id);
        gpu->setDeviceId(enumeratedDevices[i]->deviceinfo.pci->device_id);

        gpu->setVendorName(pciQuery.queryVendorName(gpu->getVendorId()));
        gpu->setDeviceName(pciQuery.queryDeviceName(gpu->getVendorId(), gpu->getDeviceId()));

        // Load the driver supported options
        const char *driverOptions = (*(this->getDriverConfig))(gpu->getDriverName().c_str());

        // If for some reason mesa is unable to query the options we simply skip this gpu
        if (driverOptions == nullptr) {
            std::cerr << Glib::ustring::compose(
                    _("Unable to extract configuration for driver %1"), gpu->getDriverName()
            ) << std::endl;

            continue;
        }

        Glib::ustring options(driverOptions);
        if (options.empty()) {
            std::cerr << Glib::ustring::compose(
                    _("Unable to extract configuration for driver %1"), gpu->getDriverName()
            ) << std::endl;

            continue;
        }

        auto parsedSections = Parser::parseAvailableConfiguration(options, locale);
        gpu->setSections(parsedSections);

        gpus[gpu->getPciId()] = gpu;
    }

    drmFreeDevices(enumeratedDevices, deviceCount);

    return gpus;
}

