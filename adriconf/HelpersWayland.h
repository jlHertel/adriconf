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

    void setup_x(const char *display_name,
            Display** out_display,
            int screenXlib,
            Window* out_window);
    void setup_egl(
            EGLint api,
            EGLNativeDisplayType native_display,
            EGLNativeWindowType native_window,
            EGLDisplay* out_display,
            EGLConfig* out_config,
            EGLContext* out_context,
            EGLSurface* out_window_surface);

public:
    HelpersWayland() {}
    bool hasProperLibEGL();
    const char *queryDriverName(int screen);
    const char *queryDriverConfig(const char *dn);
};

#endif //ENABLE_XWAYLAND
#endif //HELPERS_WAYLAND
