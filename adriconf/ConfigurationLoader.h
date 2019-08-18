#ifndef ADRICONF_CONFIGURATIONLOADER_H
#define ADRICONF_CONFIGURATIONLOADER_H

#include <glibmm/ustring.h>
#include <memory>
#include <map>
#include <boost/filesystem.hpp>

#include "DriverConfiguration.h"
#include "Device.h"
#include "GPUInfo.h"
#include "DRIQuery.h"

class ConfigurationLoader {
private:
    Glib::ustring readSystemWideXML();

    Glib::ustring readUserDefinedXML();

    DRIQuery driQuery;
    LoggerInterface *logger;

public:
    std::list<DriverConfiguration> loadDriverSpecificConfiguration(const Glib::ustring &locale);

    std::list<Device_ptr> loadSystemWideConfiguration();

    std::list<Device_ptr> loadUserDefinedConfiguration();

    std::map<Glib::ustring, GPUInfo_ptr> loadAvailableGPUs(const Glib::ustring &locale);

    Glib::ustring getOldSystemWideConfigurationPath();

    boost::filesystem::path getSystemWideConfigurationPath();

    ConfigurationLoader(const DRIQuery &driQuery, LoggerInterface *logger);
};

#endif