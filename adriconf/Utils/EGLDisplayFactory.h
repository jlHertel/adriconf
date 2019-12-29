#ifndef ADRICONF_EGLDISPLAYFACTORY_H
#define ADRICONF_EGLDISPLAYFACTORY_H

#include "EGLDisplayFactoryInterface.h"
#include "TranslatorInterface.h"

#include <functional>

#include <EGL/egl.h>
#include <boost/shared_ptr.hpp>

class EGLDisplayFactory : public EGLDisplayFactoryInterface {
private:
    std::function<EGLDisplay(EGLenum, void *, const EGLint *)> getPlatformDisplay;
    TranslatorInterface *translator;

public:
    explicit EGLDisplayFactory(TranslatorInterface *translator);

    virtual ~EGLDisplayFactory() = default;

    std::shared_ptr<EGLDisplayInterface> createDisplayFromGBM(const GBMDevice &device) override;
};


#endif //ADRICONF_EGLDISPLAYFACTORY_H
