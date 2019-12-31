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
public:
    DRIQuery(
            LoggerInterface *logger,
            TranslatorInterface *translator,
            ParserInterface *parser,
            PCIDatabaseQueryInterface *pciQuery,
            DRMDeviceFactoryInterface *drmDeviceFactory,
            GBMDeviceFactoryInterface *gbmUtils,
            EGLDisplayFactoryInterface *eglWrapper
    );

    std::map<Glib::ustring, GPUInfo_ptr> enumerateDRIDevices(const Glib::ustring &locale);
};

#endif
