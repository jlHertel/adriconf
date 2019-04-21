#include "ConfigurationLoader.h"

#include <fstream>

Glib::ustring ConfigurationLoader::readSystemWideXML() {
    Glib::ustring container;
    std::ostringstream buffer;
    std::ifstream input(this->getOldSystemWideConfigurationPath());

    if (!input.good()) {
        return std::move(container);
    }

    buffer << input.rdbuf();
    container = buffer.str();

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

std::list<Device_ptr> ConfigurationLoader::loadSystemWideConfiguration() {
    std::list<Device_ptr> systemWideDevices;

    Glib::ustring systemWideXML = this->readSystemWideXML();
    if (!systemWideXML.empty()) {
        systemWideDevices = Parser::parseDevices(systemWideXML);
    }

    /* In case no configuration is available system-wide we generate an empty one */
    if (systemWideDevices.empty()) {
        Device_ptr fakeDevice = std::make_shared<Device>();
        systemWideDevices.emplace_back(fakeDevice);
    }

    return systemWideDevices;
}

std::list<Device_ptr> ConfigurationLoader::loadUserDefinedConfiguration() {
    Glib::ustring userDefinedXML(this->readUserDefinedXML());
    if (userDefinedXML.empty()) {
        std::list<Device_ptr> deviceList;
        return deviceList;
    }
    return Parser::parseDevices(userDefinedXML);
}

Glib::ustring ConfigurationLoader::getOldSystemWideConfigurationPath() {
    Glib::ustring path("/etc/drirc");
    std::ifstream flatpakInfo("/.flatpak-info");

    if(flatpakInfo.good()) {
        path = "/var/run/host/etc/drirc";
    }

    return path;
}