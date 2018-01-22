#ifndef ADRICONF_CONFIGURATIONLOADER_H
#define ADRICONF_CONFIGURATIONLOADER_H

#include <glibmm/ustring.h>
#include <memory>

#include "DriverConfiguration.h"
#include "Device.h"

class ConfigurationLoader {
private:
    Glib::ustring readSystemWideXML();

    Glib::ustring readUserDefinedXML();

public:
    std::list<DriverConfiguration> loadDriverSpecificConfiguration(const Glib::ustring &locale);

    std::shared_ptr<Device> loadSystemWideConfiguration();

    std::list<std::shared_ptr<Device>> loadUserDefinedConfiguration();
};

#endif
