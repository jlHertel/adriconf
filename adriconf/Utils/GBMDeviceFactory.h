#ifndef ADRICONF_GBMDEVICEFACTORY_H
#define ADRICONF_GBMDEVICEFACTORY_H

#include "GBMDeviceFactoryInterface.h"
#include "../Translation/TranslatorInterface.h"

class GBMDeviceFactory : public GBMDeviceFactoryInterface {
private:
    TranslatorInterface *translator;
public:
    explicit GBMDeviceFactory(TranslatorInterface *translator) : translator(translator) {}

    GBMDevice generateDeviceFromPath(const char *path) override;
};

#endif //ADRICONF_GBMDEVICEFACTORY_H