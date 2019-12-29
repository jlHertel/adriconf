#ifndef ADRICONF_EGLDISPLAYINTERFACE_H
#define ADRICONF_EGLDISPLAYINTERFACE_H

class EGLDisplayInterface {
public:
    virtual const char *getDriverName() = 0;

    virtual const char *getDriverOptions() = 0;
};

#endif //ADRICONF_EGLDISPLAYINTERFACE_H