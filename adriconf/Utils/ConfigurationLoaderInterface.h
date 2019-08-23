#ifndef ADRICONF_CONFIGURATIONLOADERINTERFACE_H
#define ADRICONF_CONFIGURATIONLOADERINTERFACE_H


#include <glibmm/ustring.h>
#include <memory>
#include <map>
#include <boost/filesystem.hpp>
#include "../ValueObject/DriverConfiguration.h"
#include "../ValueObject/Device.h"
#include "../ValueObject/GPUInfo.h"
#include "DRIQuery.h"

class ConfigurationLoaderInterface {

public:
    virtual std::list<DriverConfiguration> loadDriverSpecificConfiguration(const Glib::ustring &locale) = 0;

    virtual std::map<Glib::ustring, GPUInfo_ptr> loadAvailableGPUs(const Glib::ustring &locale) = 0;

    virtual std::list<Device_ptr> loadSystemWideConfiguration() = 0;

    virtual std::list<Device_ptr> loadUserDefinedConfiguration() = 0;

    virtual Glib::ustring getOldSystemWideConfigurationPath() = 0;

    virtual boost::filesystem::path getSystemWideConfigurationPath() = 0;

    virtual ~ConfigurationLoaderInterface() {}
};


#endif //ADRICONF_CONFIGURATIONLOADERINTERFACE_H
