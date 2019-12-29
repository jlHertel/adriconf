#ifdef ENABLE_XWAYLAND

#include "HelpersWayland.h"

bool HelpersWayland::hasProperLibEGL() {
    driverName = (eglGetDisplayDriverName_t *) eglGetProcAddress("eglGetDisplayDriverName");
    driverConfig = (eglGetDisplayDriverconfig_t *) eglGetProcAddress("eglGetDisplayDriverConfig");

    if (!driverName || !driverConfig) {
        this->logger->error(
                this->translator->trns("Error getting function pointers. LibEGL must be too old.")
        );
        return false;
    }

    return true;
}

const char *HelpersWayland::queryDriverName() {
    EGLDisplay egl_display;
    EGLint egl_major, egl_minor;
    const char *driver_name;

    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (!eglInitialize(egl_display, &egl_major, &egl_minor)) {
        this->logger->error(
                this->translator->trns("eglInitialize() failed")
        );
        return nullptr;
    }

    driverName = (eglGetDisplayDriverName_t *) eglGetProcAddress("eglGetDisplayDriverName");
    driver_name = (*driverName)(egl_display);

    return driver_name;
}

const char *HelpersWayland::queryDriverConfig() {
    EGLDisplay egl_display;
    EGLint egl_major, egl_minor;

    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (!eglInitialize(egl_display, &egl_major, &egl_minor)) {
        this->logger->error(
                this->translator->trns("eglInitialize() failed")
        );
        return nullptr;
    }
    driverConfig = (eglGetDisplayDriverconfig_t *) eglGetProcAddress("eglGetDisplayDriverConfig");
    const char *config = (*driverConfig)(egl_display);

    return config;
}

#endif // ENABLE_XWAYLAND
