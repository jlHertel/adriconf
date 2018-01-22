#ifndef DRICONF3_PARSER_H
#define DRICONF3_PARSER_H

#include <glibmm/ustring.h>
#include "Section.h"
#include "Device.h"
#include <libxml++/libxml++.h>
#include <list>

namespace Parser {
    std::list<Section> parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale);

    DriverOption parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale);

    std::list<std::shared_ptr<Device>> parseDevices(Glib::ustring &xml);

    std::shared_ptr<Application> parseApplication(xmlpp::Node *application);

    std::list<DriverOption> convertSectionsToOptionsObject(const std::list<Section> &sections);

    std::list<Glib::ustring> convertSectionsToOptions(const std::list<Section> &sections);
};

#endif