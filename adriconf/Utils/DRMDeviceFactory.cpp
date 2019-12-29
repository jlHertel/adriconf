#include "DRMDeviceFactory.h"
#include "../ValueObject/DRMDevice.h"

#include <xf86drm.h>

std::vector<std::shared_ptr<DRMDeviceInterface>> DRMDeviceFactory::getDevices() {
    std::vector<std::shared_ptr<DRMDeviceInterface>> devices;
    drmDevicePtr enumeratedDevices[ADRICONF_MESA_MAX_DRM_DEVICES];
    int deviceCount = drmGetDevices2(0, enumeratedDevices, ADRICONF_MESA_MAX_DRM_DEVICES);
    for (int i = 0; i < deviceCount; i++) {
        auto device = std::make_shared<DRMDevice>(enumeratedDevices[i]);

        devices.emplace_back(device);
    }

    return devices;
}
