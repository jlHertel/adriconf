#include <giomm.h>
#include <xf86drm.h>
#include <iomanip>
#include <fcntl.h>

#include "DRIQuery.h"
#include "PCIDatabaseQuery.h"


DRIQuery::DRIQuery() {
    this->getScreenDriver = (glXGetScreenDriver_t *) glXGetProcAddress((const GLubyte *) "glXGetScreenDriver");
    this->getDriverConfig = (glXGetDriverConfig_t *) glXGetProcAddress((const GLubyte *) "glXGetDriverConfig");

    if (!this->getScreenDriver || !this->getDriverConfig) {
        std::cerr << "Error getting function pointers. LibGL must be too old." << std::endl;
    }
}

std::list<DriverConfiguration> DRIQuery::queryDriverConfigurationOptions(const Glib::ustring &locale) {
    std::list<DriverConfiguration> configurations;

    Display *display;

    if (!(display = XOpenDisplay(nullptr))) {
        std::cerr << "Couldn't open X display" << std::endl;
        return configurations;
    }

    /* Call glXGetClientString to load vendor libs on glvnd enabled systems */
    glXGetClientString(display, GLX_EXTENSIONS);

    int screenCount = ScreenCount (display);

    for (int i = 0; i < screenCount; i++) {
        DriverConfiguration config;
        config.setScreen(i);

        auto driverName = (*(this->getScreenDriver))(display, i);
        config.setDriver(driverName);

        auto driverOptions = (*(this->getDriverConfig))(driverName);
        Glib::ustring options(driverOptions);

        auto parsedSections = Parser::parseAvailableConfiguration(options, locale);
        config.setSections(parsedSections);

        configurations.emplace_back(config);
    }

    XCloseDisplay(display);

    return std::move(configurations);
}

std::list<GPUInfo_ptr> DRIQuery::enumerateDRIDevices() {
    std::list<GPUInfo_ptr> gpus;

    PCIDatabaseQuery pciQuery;

    drmDevicePtr enumeratedDevices[MESA_MAX_DRM_DEVICES];
    int deviceCount = drmGetDevices2(0, enumeratedDevices, MESA_MAX_DRM_DEVICES);
    std::cout << "Enumerated a total of " << deviceCount << " devices" << std::endl;

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

        gpu->setVendorName(pciQuery.queryVendorName(enumeratedDevices[i]->deviceinfo.pci->vendor_id));
        gpu->setDeviceName(pciQuery.queryDeviceName(
                enumeratedDevices[i]->deviceinfo.pci->vendor_id,
                enumeratedDevices[i]->deviceinfo.pci->device_id
        ));

        std::cout << std::endl << "****" << std::endl;
        std::cout << "PCI ID: " << gpu->getPciId() << std::endl;
        std::cout << "Vendor name: " << gpu->getVendorName() << std::endl;
        std::cout << "Device name: " << gpu->getDeviceName() << std::endl;
        std::cout << "Driver name: " << gpu->getDriverName() << std::endl;

        gpus.emplace_back(gpu);
    }

    drmFreeDevices(enumeratedDevices, deviceCount);

    return gpus;
}

