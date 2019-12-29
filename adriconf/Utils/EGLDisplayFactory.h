#ifndef ADRICONF_EGLDISPLAYFACTORY_H
#define ADRICONF_EGLDISPLAYFACTORY_H

#include "EGLDisplayFactoryInterface.h"

#include <functional>

#include <EGL/egl.h>
#include <boost/shared_ptr.hpp>

class EGLDisplayFactory : public EGLDisplayFactoryInterface {
private:
    std::function<EGLDisplay(EGLenum, void *, const EGLint *)> getPlatformDisplay;

public:
    EGLDisplayFactory();

    virtual ~EGLDisplayFactory() = default;

    std::shared_ptr<EGLDisplayInterface> createDisplayFromGBM(const GBMDevice &device) override;
};


#endif //ADRICONF_EGLDISPLAYFACTORY_H
