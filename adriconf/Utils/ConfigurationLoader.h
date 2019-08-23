#ifndef ADRICONF_CONFIGURATIONLOADER_H
#define ADRICONF_CONFIGURATIONLOADER_H

#include <glibmm/ustring.h>
#include <memory>
#include <map>
#include <boost/filesystem.hpp>

#include "../ValueObject/DriverConfiguration.h"
#include "../ValueObject/Device.h"
#include "../ValueObject/GPUInfo.h"
#include "DRIQuery.h"
#include "ConfigurationLoaderInterface.h"

class ConfigurationLoader : public ConfigurationLoaderInterface {
private:
    Glib::ustring readSystemWideXML();

    Glib::ustring readUserDefinedXML();

    DRIQuery driQuery;
    LoggerInterface *logger;
    ParserInterface *parser;

public:
    std::list<DriverConfiguration> loadDriverSpecificConfiguration(const Glib::ustring &locale) override;

    std::list<Device_ptr> loadSystemWideConfiguration() override;

    std::list<Device_ptr> loadUserDefinedConfiguration() override;

    std::map<Glib::ustring, GPUInfo_ptr> loadAvailableGPUs(const Glib::ustring &locale) override;

    Glib::ustring getOldSystemWideConfigurationPath() override;

    boost::filesystem::path getSystemWideConfigurationPath() override;

    ConfigurationLoader(const DRIQuery &driQuery, LoggerInterface *logger, ParserInterface *parser);

    virtual ~ConfigurationLoader() {};
};

#endif