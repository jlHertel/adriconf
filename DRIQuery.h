#ifndef DRICONF3_DRIQUERY_H
#define DRICONF3_DRIQUERY_H

#include "DriverConfiguration.h"
#include "Parser.h"

#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xlib.h>
#include <glibmm/ustring.h>
#include "GPUInfo.h"

/* MESA HAS THIS HARD-CODED SO WE MUST HARD-CODE IT ALSO */
#define MESA_MAX_DRM_DEVICES 32

typedef const char *glXGetScreenDriver_t(Display *dpy, int scrNum);

typedef const char *glXGetDriverConfig_t(const char *driverName);

typedef const char *glXQueryExtensionsString_t(Display *dpy, int screen);

typedef Bool *glXQueryRenderer_t(Display *dpy, int screen, int renderer, int attribute, unsigned int *value);

class DRIQuery {
private:
    glXGetScreenDriver_t *getScreenDriver;
    glXGetDriverConfig_t *getDriverConfig;
    glXQueryRenderer_t *getRendererInfo;
    glXQueryExtensionsString_t *getGlxExtensionsString;

public:
    DRIQuery();

    bool canHandle();

    std::list<DriverConfiguration> queryDriverConfigurationOptions(const Glib::ustring &locale);

    std::map<Glib::ustring, GPUInfo_ptr> enumerateDRIDevices(const Glib::ustring &locale);
};

#endif
