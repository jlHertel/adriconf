#ifdef ENABLE_XWAYLAND

#ifndef HELPERS_WAYLAND
#define HELPERS_WAYLAND

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include <EGL/egl.h>

typedef const char *eglGetDisplayDriverName_t(EGLDisplay disp);
typedef const char *eglGetDisplayDriverconfig_t(EGLDisplay disp);

class HelpersWayland
{
private:
    eglGetDisplayDriverName_t *driverName;
    eglGetDisplayDriverconfig_t *driverConfig;

public:
    HelpersWayland() {}
    bool hasProperLibEGL();
    const char *queryDriverName();
    const char *queryDriverConfig();
};

#endif //ENABLE_XWAYLAND
#endif //HELPERS_WAYLAND
