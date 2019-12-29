#ifndef ADRICONF_EGLDISPLAYWRAPPER_H
#define ADRICONF_EGLDISPLAYWRAPPER_H

#include "EGLDisplayInterface.h"

#include <functional>
#include <EGL/egl.h>
#include <glibmm/ustring.h>

class EGLDisplayWrapper : public EGLDisplayInterface {
private:
    EGLDisplay rawDisplay;
    EGLint minorVersion;
    EGLint majorVersion;
    Glib::ustring extensions;
    std::function<const char *(EGLDisplay)> queryDriverName;
    std::function<const char *(EGLDisplay)> queryDriverOptions;

    bool hasMesaQueryDriverExtension() const;

public:
    EGLDisplayWrapper();

    virtual ~EGLDisplayWrapper();

    void setRawDisplay(EGLDisplay rawDisplay);

    void setMinorVersion(EGLint minorVersion);

    void setMajorVersion(EGLint majorVersion);

    void setExtensions(const Glib::ustring &extensions);

    const char *getDriverName() override;

    const char *getDriverOptions() override;
};

#endif //ADRICONF_EGLDISPLAYWRAPPER_H