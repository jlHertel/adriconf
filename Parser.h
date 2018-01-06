#ifndef DRICONF3_PARSER_H
#define DRICONF3_PARSER_H

#include <glibmm/ustring.h>
#include "Section.h"
#include "Device.h"
#include <libxml++/libxml++.h>
#include <list>

namespace DRI::Parser {
    std::list<DRI::Section> parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale);

    DRI::DriverOption parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale);

    std::list<DRI::Device*> parseDevices(Glib::ustring &xml);

    DRI::Application* parseApplication(xmlpp::Node *application);

    std::list<DRI::DriverOption> convertSectionsToOptionsObject(const std::list<DRI::Section> &sections);

    std::list<Glib::ustring> convertSectionsToOptions(const std::list<DRI::Section> &sections);
};

#endif