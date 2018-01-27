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

class DRIQuery {
private:
    glXGetScreenDriver_t *getScreenDriver;
    glXGetDriverConfig_t *getDriverConfig;

public:
    DRIQuery();

    std::list<DriverConfiguration> queryDriverConfigurationOptions(const Glib::ustring &locale);

    std::list<GPUInfo_ptr> enumerateDRIDevices();
};

#endif