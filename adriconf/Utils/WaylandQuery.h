#ifndef ADRICONF_WAYLANDQUERY_H
#define ADRICONF_WAYLANDQUERY_H

#include "DisplayServerQueryInterface.h"
#include "EGLDisplayFactoryInterface.h"
#include "../Logging/LoggerInterface.h"
#include "../Translation/TranslatorInterface.h"
#include "ParserInterface.h"

class WaylandQuery : public DisplayServerQueryInterface {
private:
    LoggerInterface *logger;
    TranslatorInterface *translator;
    ParserInterface *parser;
    EGLDisplayFactoryInterface *eglDisplayFactory;

public:
    WaylandQuery(
            LoggerInterface *logger,
            TranslatorInterface *translator,
            ParserInterface *parse,
            EGLDisplayFactoryInterface *eglDisplayFactory
    );

    std::list<DriverConfiguration> queryDriverConfigurationOptions(const Glib::ustring &locale) override;

    bool checkNecessaryExtensions() override;
};

#endif //ADRICONF_WAYLANDQUERY_H