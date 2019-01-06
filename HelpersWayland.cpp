#ifdef ENABLE_XWAYLAND

#include <iostream>
#include <X11/Xlib.h>
#include <glibmm/i18n.h>

#include "HelpersWayland.h"

bool HelpersWayland::hasProperLibEGL() {
    driverName = (eglGetDisplayDriverName_t *) eglGetProcAddress("eglGetDisplayDriverName");
    driverConfig = (eglGetDisplayDriverconfig_t *) eglGetProcAddress("eglGetDisplayDriverConfig");

    if (!driverName || !driverConfig) {
        std::cerr << _("Error getting function pointers. LibEGL must be too old.\n");
        return false;
    }

    return true;
}

void HelpersWayland::setup_x(const char *display_name,
        Display** out_display,
        int screenXlib,
        Window* out_window) {


    Display* display = XOpenDisplay(display_name);
    if (!display)
         std::cerr << ("XOpenDisplay() failed\n");

    Window w = XCreateSimpleWindow(
                display, XDefaultRootWindow(display),
                0, 0, 64, 64, 4, 0, 0);

    XMapWindow(display, w);

    *out_display = display;
    *out_window = w;
}

void HelpersWayland::setup_egl(
        EGLint api,
        EGLNativeDisplayType native_display,
        EGLNativeWindowType native_window,
        EGLDisplay* out_display,
        EGLConfig* out_config,
        EGLContext* out_context,
        EGLSurface* out_window_surface) {

    EGLint ignore;
    EGLBoolean ok;

    ok = eglBindAPI(api);
    if (!ok)
        std::cerr << _("eglBindAPI(0x%x) failed\n");

    EGLDisplay display = eglGetDisplay(native_display);
    if (display == EGL_NO_DISPLAY)
        std::cerr << _("eglGetDisplay() failed\n");

    ok = eglInitialize(display, &ignore, &ignore);
    if (!ok)
        std::cerr << _("eglInitialize() failed\n");

    EGLint configs_size = 256;
    EGLConfig* configs = new EGLConfig[configs_size];
    EGLint num_configs;
    ok = eglChooseConfig(
        display,
        NULL,
        configs,
        configs_size, // num requested configs
        &num_configs); // num returned configs
    if (!ok)
        std::cerr << _("eglChooseConfig() failed\n");
    if (num_configs == 0)
        std::cerr << _("failed to find suitable EGLConfig\n");
    EGLConfig config = configs[0];
    delete [] configs;

    EGLContext context = eglCreateContext(
        display,
        config,
        EGL_NO_CONTEXT,
        NULL);
    if (!context)
        std::cerr << _("eglCreateContext() failed\n");

    EGLSurface surface = eglCreateWindowSurface(
        display,
        config,
        native_window,
        NULL);
    if (!surface)
        std::cerr << _("eglCreateWindowSurface() failed\n");

    ok = eglMakeCurrent(display, surface, surface, context);
    if (!ok)
    std::cerr << _("eglMakeCurrent() failed\n");

    *out_display = display;
    *out_config = config;
    *out_context = context;
    *out_window_surface = surface;
}

const char *HelpersWayland::queryDriverName(int screen) {
    const char *x_display_name = NULL;
    Display *x_display;
    int x_screen = screen;
    Window x_window;

    const char *ret;
    setup_x(x_display_name,
            &x_display,
            x_screen,
            &x_window);

    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
    EGLSurface egl_surface;
    setup_egl(EGL_OPENGL_API,
              x_display,
              x_window,
              &egl_display,
              &egl_config,
              &egl_context,
              &egl_surface);


    driverName = (eglGetDisplayDriverName_t *) eglGetProcAddress("eglGetDisplayDriverName");
    ret = (* driverName) (egl_display);

    XCloseDisplay(x_display);
    return ret;
}

const char *HelpersWayland::queryDriverConfig(const char *dn) {
    const char *x_display_name = NULL;
    Display *x_display;
    int x_screen = 0;
    Window x_window;
    setup_x(x_display_name,
            &x_display,
            x_screen,
            &x_window);

    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
    EGLSurface egl_surface;
    setup_egl(EGL_OPENGL_API,
              x_display,
              x_window,
              &egl_display,
              &egl_config,
              &egl_context,
              &egl_surface);


    driverConfig = (eglGetDisplayDriverconfig_t *) eglGetProcAddress("eglGetDisplayDriverConfig");
    const char * config = (* driverConfig) (egl_display);

    XCloseDisplay(x_display);
    return config;
}

#endif // ENABLE_XWAYLAND
