#ifndef ADRICONF_GBMDEVICEFACTORYINTERFACE_H
#define ADRICONF_GBMDEVICEFACTORYINTERFACE_H

#include "../ValueObject/GBMDevice.h"

class GBMDeviceFactoryInterface {
public:
    virtual GBMDevice generateDeviceFromPath(const char *path) = 0;
};

#endif //ADRICONF_GBMDEVICEFACTORYINTERFACE_H
