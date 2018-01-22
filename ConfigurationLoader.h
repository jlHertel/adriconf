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

    Device_ptr loadSystemWideConfiguration();

    std::list<Device_ptr> loadUserDefinedConfiguration();
};

#endif
