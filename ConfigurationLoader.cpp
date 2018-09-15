#include "ConfigurationLoader.h"

#include <fstream>

Glib::ustring ConfigurationLoader::readSystemWideXML() {
    std::ostringstream buffer;
    std::ifstream input("/etc/drirc");
    buffer << input.rdbuf();
    Glib::ustring container(buffer.str());

    return std::move(container);
}

Glib::ustring ConfigurationLoader::readUserDefinedXML() {
    Glib::ustring container;

    std::string userHome(std::getenv("HOME"));
    std::ifstream input(userHome + "/.drirc");

    if (!input.good()) {
        return container;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();

    container = buffer.str();

    return container;
}

std::list<DriverConfiguration> ConfigurationLoader::loadDriverSpecificConfiguration(const Glib::ustring &locale) {
    return this->driQuery.queryDriverConfigurationOptions(locale);
}

std::map<Glib::ustring, GPUInfo_ptr> ConfigurationLoader::loadAvailableGPUs(const Glib::ustring &locale) {
    return this->driQuery.enumerateDRIDevices(locale);
}

Device_ptr ConfigurationLoader::loadSystemWideConfiguration() {
    Glib::ustring systemWideXML = this->readSystemWideXML();
    std::list<Device_ptr> systemWideDevices = Parser::parseDevices(systemWideXML);

    /* In case no configuration is available system-wide we generate an empty one */
    if (systemWideDevices.empty()) {
        auto fakeDevice = std::make_shared<Device>();
        systemWideDevices.emplace_back(fakeDevice);
    }

    return systemWideDevices.front();
}

std::list<Device_ptr> ConfigurationLoader::loadUserDefinedConfiguration() {
    Glib::ustring userDefinedXML(this->readUserDefinedXML());
    if (userDefinedXML.empty()) {
        std::list<Device_ptr> deviceList;
        return deviceList;
    }
    return Parser::parseDevices(userDefinedXML);
}
