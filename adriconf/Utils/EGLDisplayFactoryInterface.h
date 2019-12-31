#ifndef ADRICONF_EGLDISPLAYFACTORYINTERFACE_H
#define ADRICONF_EGLDISPLAYFACTORYINTERFACE_H

#include "../ValueObject/EGLDisplayInterface.h"
#include "../ValueObject/GBMDevice.h"
#include <memory>

class EGLDisplayFactoryInterface {
public:
    virtual std::shared_ptr<EGLDisplayInterface> createDisplayFromGBM(const GBMDevice &device) = 0;
    virtual std::shared_ptr<EGLDisplayInterface> createDefaultDisplay() = 0;
};

#endif //ADRICONF_EGLDISPLAYFACTORYINTERFACE_H