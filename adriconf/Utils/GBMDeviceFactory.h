#ifndef ADRICONF_GBMDEVICEFACTORY_H
#define ADRICONF_GBMDEVICEFACTORY_H

#include "GBMDeviceFactoryInterface.h"

class GBMDeviceFactory : public GBMDeviceFactoryInterface {
public:
    GBMDevice generateDeviceFromPath(const char *path) override;
};


#endif //ADRICONF_GBMDEVICEFACTORY_H
