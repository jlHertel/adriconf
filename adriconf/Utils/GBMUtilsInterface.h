#ifndef ADRICONF_GBMUTILSINTERFACE_H
#define ADRICONF_GBMUTILSINTERFACE_H

#include "../ValueObject/GBMDevice.h"

class GBMUtilsInterface {
public:
    virtual GBMDevice generateDeviceFromPath(const char *path) = 0;
};

#endif //ADRICONF_GBMUTILSINTERFACE_H
