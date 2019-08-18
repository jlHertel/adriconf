#ifndef DRICONF3_PARSER_H
#define DRICONF3_PARSER_H

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>
#include <list>
#include <iostream>

#include "Section.h"
#include "Device.h"
#include "LoggerInterface.h"
#include "ParserInterface.h"

class Parser : public ParserInterface {
private:
    LoggerInterface *logger;

public:
    Parser(LoggerInterface *logger) : logger(logger) {}

    ~Parser() override {}

    std::list<Section> parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale) override;

    DriverOption parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale) override;

    std::list<Device_ptr> parseDevices(Glib::ustring &xml) override;

    Application_ptr parseApplication(xmlpp::Node *application) override;
};

#endif
