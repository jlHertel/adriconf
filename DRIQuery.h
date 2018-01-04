#ifndef DRICONF3_DRIQUERY_H
#define DRICONF3_DRIQUERY_H

#include "DriverConfiguration.h"
#include "Parser.h"

#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xlib.h>
#include <glibmm/ustring.h>

typedef const char * glXGetScreenDriver_t (Display *dpy, int scrNum);
typedef const char * glXGetDriverConfig_t (const char *driverName);

namespace DRI {
    class DRIQuery {
    private:
        glXGetScreenDriver_t *getScreenDriver;
        glXGetDriverConfig_t *getDriverConfig;

    public:
        DRIQuery();

        std::list<DRI::DriverConfiguration> queryDriverConfigurationOptions(const Glib::ustring &locale);
    };
};

#endif