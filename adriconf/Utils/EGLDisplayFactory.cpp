#include "EGLDisplayFactory.h"
#include "../ValueObject/EGLDisplayWrapper.h"
#include <EGL/eglext.h>
#include <stdexcept>
#include <glibmm/i18n.h>

EGLDisplayFactory::EGLDisplayFactory() {
    this->getPlatformDisplay = (EGLDisplay (*)(EGLenum, void *, const EGLint *)) eglGetProcAddress(
            "eglGetPlatformDisplayEXT");
}

std::shared_ptr<EGLDisplayInterface> EGLDisplayFactory::createDisplayFromGBM(const GBMDevice &device) {
    EGLDisplay display = this->getPlatformDisplay(EGL_PLATFORM_GBM_MESA, device.getRawGBMDevice(), nullptr);
    if (display == EGL_NO_DISPLAY) {
        throw std::runtime_error(_("Failed to create EGL display"));
    }

    EGLint major = 0, minor = 0;
    EGLBoolean initialized = eglInitialize(display, &major, &minor);
    if (initialized == EGL_FALSE) {
        throw std::runtime_error(_("Failed to initialize EGL display"));
    }

    const char *extensions = eglQueryString(display, EGL_EXTENSIONS);
    std::string extStr(extensions);

    auto wrapper = std::make_shared<EGLDisplayWrapper>();
    wrapper->setRawDisplay(display);
    wrapper->setMajorVersion(major);
    wrapper->setMinorVersion(minor);
    wrapper->setExtensions(extStr);

    return wrapper;
}
