#ifndef ADRICONF_PARSERMOCK_H
#define ADRICONF_PARSERMOCK_H

#include "gmock/gmock.h"
#include "../../adriconf/Utils/ParserInterface.h"

class ParserMock : public ParserInterface {
public:
    MOCK_METHOD2(parseAvailableConfiguration, std::list<Section>(
            const Glib::ustring &xml,
            const Glib::ustring &currentLocale));

    MOCK_METHOD2(parseSectionOptions, DriverOption(xmlpp::Node * option,
            const Glib::ustring &currentLocale));

    MOCK_METHOD1(parseDevices, std::list<Device_ptr>(Glib::ustring & xml));

    MOCK_METHOD1(parseApplication, Application_ptr(xmlpp::Node * application));
};

#endif //ADRICONF_PARSERMOCK_H
