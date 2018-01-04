#ifndef DRICONF3_PARSER_H
#define DRICONF3_PARSER_H

#include <glibmm/ustring.h>
#include "Section.h"
#include "Device.h"
#include <libxml++/libxml++.h>
#include <list>

namespace DRI::Parser {
    std::list<DRI::Section> parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale);

    DRI::Option parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale);

    std::list<DRI::Device> parseDevices(Glib::ustring &xml);

    DRI::Application parseApplication(xmlpp::Node *application);

    std::list<Glib::ustring> convertSectionsToOptions(std::list<DRI::Section> sections);

    void printContainer(std::list<DRI::Device> devices);
};

#endif