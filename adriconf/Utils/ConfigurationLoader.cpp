#include "ConfigurationLoader.h"
#include "ConfigurationResolver.h"

#include <boost/filesystem.hpp>
#include <glibmm/i18n.h>
#include <fstream>
#include <algorithm>

Glib::ustring ConfigurationLoader::readSystemWideXML() {
    this->logger->debug(_("Reading legacy system-wide XML"));
    Glib::ustring container;
    std::ostringstream buffer;
    Glib::ustring filePath(this->getOldSystemWideConfigurationPath());

    this->logger->debug(Glib::ustring::compose(_("Legacy system-wide XML path: %1"), filePath));
    std::ifstream input(filePath);

    if (!input.good()) {
        this->logger->debug(_("Legacy system-wide file doesn't exist"));
        return container;
    }

    buffer << input.rdbuf();
    container = buffer.str();

    return container;
}

Glib::ustring ConfigurationLoader::readUserDefinedXML() {
    this->logger->debug(_("Reading user defined XML"));
    Glib::ustring container;

    std::string userHome(std::getenv("HOME"));

    this->logger->debug(Glib::ustring::compose(_("User defined XML path: %1"), userHome + "/.drirc"));
    std::ifstream input(userHome + "/.drirc");

    if (!input.good()) {
        this->logger->debug(_("User defined XML doesn't exist"));
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
    this->logger->debug(_("Reading system-wide XML"));
    std::list<Device_ptr> systemWideDevices;

    std::vector<Glib::ustring> configurationPaths;
    boost::filesystem::path configurationPath = this->getSystemWideConfigurationPath();
    this->logger->debug(Glib::ustring::compose(_("System-wide XML path: %1"), configurationPath.c_str()));

    for (const auto &file : boost::filesystem::directory_iterator(configurationPath)) {
        if (!boost::filesystem::is_directory(file)) {
            configurationPaths.emplace_back(file.path().string());
        }
    }

    std::sort(configurationPaths.begin(), configurationPaths.end());

    for (auto &filename : configurationPaths) {
        this->logger->debug(Glib::ustring::compose(_("Found configuration on path: %1"), filename));
        Glib::ustring container;
        std::ostringstream buffer;
        std::ifstream input(filename);
        if (!input.good()) {
            this->logger->debug(Glib::ustring::compose(_("Failed to load file: %1"), filename));
            continue;
        }

        buffer << input.rdbuf();
        container = buffer.str();
        std::list<Device_ptr> justLoadedDevices = this->parser->parseDevices(container);

        this->resolver->mergeConfigurationOnTopOf(systemWideDevices, justLoadedDevices);
    }

    Glib::ustring systemWideXML = this->readSystemWideXML();
    if (!systemWideXML.empty()) {
        std::list<Device_ptr> justLoadedDevices = this->parser->parseDevices(systemWideXML);
        this->resolver->mergeConfigurationOnTopOf(systemWideDevices, justLoadedDevices);
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
        this->logger->debug(_("User defined configuration is empty. Returning an empty object"));
        std::list<Device_ptr> deviceList;
        return deviceList;
    }
    return this->parser->parseDevices(userDefinedXML);
}

Glib::ustring ConfigurationLoader::getOldSystemWideConfigurationPath() {
    Glib::ustring path("/etc/drirc");
    std::ifstream flatpakInfo("/.flatpak-info");

    if (flatpakInfo.good()) {
        path = "/var/run/host/etc/drirc";
    }

    return path;
}

boost::filesystem::path ConfigurationLoader::getSystemWideConfigurationPath() {
    std::ifstream flatpakInfo("/.flatpak-info");

    if (flatpakInfo.good()) {
        return boost::filesystem::path("/var/run/host/usr/share/drirc.d/");
    }

    return boost::filesystem::path("/usr/share/drirc.d/");
}

ConfigurationLoader::ConfigurationLoader(const DRIQuery &driQuery, LoggerInterface *logger, ParserInterface *parser,
                                         ConfigurationResolverInterface *resolver)
        : driQuery(driQuery),
          logger(logger),
          parser(parser),
          resolver(resolver) {}
