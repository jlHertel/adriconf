#ifndef ADRICONF_DRMDEVICEFACTORYINTERFACE_H
#define ADRICONF_DRMDEVICEFACTORYINTERFACE_H

#include "../ValueObject/DRMDeviceInterface.h"

#include <memory>
#include <vector>

/* MESA HAS THIS HARD-CODED TO 32 SO WE MUST HARD-CODE IT ALSO */
#define ADRICONF_MESA_MAX_DRM_DEVICES 32

class DRMDeviceFactoryInterface {
public:
    virtual std::vector<std::shared_ptr<DRMDeviceInterface>> getDevices() = 0;
};


#endif //ADRICONF_DRMDEVICEFACTORYINTERFACE_H
