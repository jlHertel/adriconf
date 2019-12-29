#ifndef DRICONF3_DRIQUERY_H
#define DRICONF3_DRIQUERY_H

#include "../ValueObject/DriverConfiguration.h"
#include "Parser.h"
#include "../ValueObject/GPUInfo.h"
#include "HelpersWayland.h"
#include "../Logging/LoggerInterface.h"
#include "PCIDatabaseQueryInterface.h"
#include "GBMDeviceFactoryInterface.h"
#include "EGLDisplayFactoryInterface.h"
#include "DRMDeviceFactoryInterface.h"

#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xlib.h>
#include <glibmm/ustring.h>

/* MESA HAS THIS HARD-CODED SO WE MUST HARD-CODE IT ALSO */
#define MESA_MAX_DRM_DEVICES 32

typedef const char *glXGetScreenDriver_t(Display *dpy, int scrNum);

typedef const char *glXGetDriverConfig_t(const char *driverName);

typedef const char *glXQueryExtensionsString_t(Display *dpy, int screen);


class DRIQuery {
private:
    LoggerInterface *logger;
    TranslatorInterface *translator;
    ParserInterface *parser;
    PCIDatabaseQueryInterface *pciQuery;
    DRMDeviceFactoryInterface *drmDeviceFactory;
    GBMDeviceFactoryInterface *gbmDeviceFactory;
    EGLDisplayFactoryInterface *eglDisplayFactory;
    HelpersWayland waylandHelper;
    bool isWaylandSession;

    glXGetScreenDriver_t *getScreenDriver;
    glXGetDriverConfig_t *getDriverConfig;
    glXQueryExtensionsString_t *getGlxExtensionsString;

    const char *queryDriverName(int screen);

    const char *queryDriverConfig(const char *dn);

public:
    DRIQuery(
            LoggerInterface *logger,
            TranslatorInterface *translator,
            ParserInterface *parser,
            PCIDatabaseQueryInterface *pciQuery,
            DRMDeviceFactoryInterface *drmDeviceFactory,
            GBMDeviceFactoryInterface *gbmUtils,
            EGLDisplayFactoryInterface *eglWrapper,
            HelpersWayland waylandHelper,
            bool isWaylandSession
    );

    bool isSystemSupported();

    bool canHandle();

    std::list<DriverConfiguration> queryDriverConfigurationOptions(const Glib::ustring &locale);

    std::map<Glib::ustring, GPUInfo_ptr> enumerateDRIDevices(const Glib::ustring &locale);
};

#endif
