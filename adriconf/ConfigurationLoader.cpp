#include "ConfigurationLoader.h"
#include "ConfigurationResolver.h"
#include <boost/filesystem.hpp>

#include <fstream>
#include <algorithm>

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

    std::vector<Glib::ustring> configurationPaths;
    for (const auto &file : boost::filesystem::directory_iterator(this->getSystemWideConfigurationPath())) {
        if (!boost::filesystem::is_directory(file)) {
            configurationPaths.emplace_back(file.path().string());
        }
    }

    std::sort(configurationPaths.begin(), configurationPaths.end());

    for (auto &filename : configurationPaths) {
        std::cout << "Found config under: " << filename << std::endl;
        Glib::ustring container;
        std::ostringstream buffer;
        std::ifstream input(filename);
        if (!input.good()) {
            continue;
        }

        buffer << input.rdbuf();
        container = buffer.str();
        std::list<Device_ptr> justLoadedDevices = Parser::parseDevices(container);

        ConfigurationResolver::mergeConfigurationOnTopOf(systemWideDevices, justLoadedDevices);
    }

    Glib::ustring systemWideXML = this->readSystemWideXML();
    if (!systemWideXML.empty()) {
        std::list<Device_ptr> justLoadedDevices = Parser::parseDevices(systemWideXML);
        ConfigurationResolver::mergeConfigurationOnTopOf(systemWideDevices, justLoadedDevices);
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

boost::filesystem::path ConfigurationLoader::getSystemWideConfigurationPath() {
    std::ifstream flatpakInfo("/.flatpak-info");

    if(flatpakInfo.good()) {
        return boost::filesystem::path("/var/run/host/usr/share/drirc.d/");
    }

    return boost::filesystem::path("/usr/share/drirc.d/");
}