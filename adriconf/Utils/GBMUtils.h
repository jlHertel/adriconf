#ifndef ADRICONF_GBMUTILS_H
#define ADRICONF_GBMUTILS_H

#include "GBMUtilsInterface.h"

class GBMUtils : public GBMUtilsInterface {
public:
    GBMDevice generateDeviceFromPath(const char *path) override;
};


#endif //ADRICONF_GBMUTILS_H
