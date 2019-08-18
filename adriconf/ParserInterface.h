#ifndef ADRICONF_PARSERINTERFACE_H
#define ADRICONF_PARSERINTERFACE_H

#include <libxml++/libxml++.h>

#include "DriverOption.h"
#include "Device.h"
#include "Section.h"

class ParserInterface {
public:
    virtual ~ParserInterface() {}

    virtual std::list<Section>
    parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale) = 0;

    virtual DriverOption parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale) = 0;

    virtual std::list<Device_ptr> parseDevices(Glib::ustring &xml) = 0;

    virtual Application_ptr parseApplication(xmlpp::Node *application) = 0;
};


#endif //ADRICONF_PARSERINTERFACE_H
