#ifndef ADRICONF_XORGQUERY_H
#define ADRICONF_XORGQUERY_H

#include "DisplayServerQueryInterface.h"
#include "../Logging/LoggerInterface.h"
#include "../Translation/TranslatorInterface.h"
#include "ParserInterface.h"

#include <GL/glx.h>
#include <GL/glxext.h>
#include <functional>

class XorgQuery : public DisplayServerQueryInterface {
private:
    LoggerInterface *logger;
    TranslatorInterface *translator;
    ParserInterface *parser;

    std::function<const char *(Display *dpy, int scrNum)> getScreenDriverName;
    std::function<const char *(const char *driverName)> getDriverOptions;

public:
    XorgQuery(LoggerInterface *logger, TranslatorInterface *translator, ParserInterface *parser);

    std::list<DriverConfiguration> queryDriverConfigurationOptions(const Glib::ustring &locale) override;

    bool checkNecessaryExtensions() override;
};


#endif //ADRICONF_XORGQUERY_H
