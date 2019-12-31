#ifndef ADRICONF_EGLDISPLAYINTERFACE_H
#define ADRICONF_EGLDISPLAYINTERFACE_H

#include <glibmm/ustring.h>

class EGLDisplayInterface {
public:
    virtual const char *getDriverName() = 0;

    virtual const char *getDriverOptions() = 0;

    virtual Glib::ustring getExtensions() const = 0;
};

#endif //ADRICONF_EGLDISPLAYINTERFACE_H