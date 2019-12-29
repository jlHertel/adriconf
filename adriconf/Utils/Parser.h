#ifndef DRICONF3_PARSER_H
#define DRICONF3_PARSER_H

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>
#include <list>
#include <iostream>

#include "../ValueObject/Section.h"
#include "../ValueObject/Device.h"
#include "../Logging/LoggerInterface.h"
#include "ParserInterface.h"
#include "TranslatorInterface.h"

class Parser : public ParserInterface {
private:
    LoggerInterface *logger;
    TranslatorInterface *translator;

public:
    Parser(LoggerInterface *logger, TranslatorInterface *translator) : logger(logger), translator(translator) {}

    ~Parser() override = default;

    std::list<Section>
    parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale) override;

    DriverOption parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale) override;

    std::list<Device_ptr> parseDevices(Glib::ustring &xml) override;

    Application_ptr parseApplication(xmlpp::Node *application) override;
};

#endif
