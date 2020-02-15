#include "ConfigurationLoader.h"
#include "ConfigurationResolver.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <algorithm>

Glib::ustring ConfigurationLoader::readSystemWideXML() {
    this->logger->debug(this->translator->trns("Reading legacy system-wide XML"));
    Glib::ustring container;
    std::ostringstream buffer;
    Glib::ustring filePath(this->getOldSystemWideConfigurationPath());

    this->logger->debug(
            Glib::ustring::compose(
                    this->translator->trns("Legacy system-wide XML path: %1"),
                    filePath
            )
    );
    std::ifstream input(filePath);

    if (!input.good()) {
        this->logger->debug(this->translator->trns("Legacy system-wide file doesn't exist"));
        return container;
    }

    buffer << input.rdbuf();
    container = buffer.str();

    return container;
}

Glib::ustring ConfigurationLoader::readUserDefinedXML() {
    this->logger->debug(this->translator->trns("Reading user defined XML"));
    Glib::ustring container;

    std::string userHome(std::getenv("HOME"));

    this->logger->debug(
            Glib::ustring::compose(
                    this->translator->trns("User defined XML path: %1"),
                    userHome + "/.drirc"
            )
    );
    std::ifstream input(userHome + "/.drirc");

    if (!input.good()) {
        this->logger->debug(this->translator->trns("User defined XML doesn't exist"));
        return container;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();

    container = buffer.str();

    return container;
}

std::list<DriverConfiguration> ConfigurationLoader::loadDriverSpecificConfiguration(const Glib::ustring &locale) {
    return this->displayQuery->queryDriverConfigurationOptions(locale);
}

std::map<Glib::ustring, GPUInfo_ptr> ConfigurationLoader::loadAvailableGPUs(const Glib::ustring &locale) {
    return this->driQuery.enumerateDRIDevices(locale);
}

std::list<Device_ptr> ConfigurationLoader::loadSystemWideConfiguration() {
    this->logger->debug(this->translator->trns("Reading system-wide XML"));
    std::list<Device_ptr> systemWideDevices;

    std::vector<Glib::ustring> configurationPaths;
    boost::filesystem::path configurationPath = this->getSystemWideConfigurationPath();
    this->logger->debug(
            Glib::ustring::compose(
                    this->translator->trns("System-wide XML path: %1"),
                    configurationPath.c_str()
            )
    );
    if (boost::filesystem::exists(configurationPath)) {
        for (const auto &file : boost::filesystem::directory_iterator(configurationPath)) {
            if (!boost::filesystem::is_directory(file)) {
                configurationPaths.emplace_back(file.path().string());
            }
        }

        std::sort(configurationPaths.begin(), configurationPaths.end());

        for (auto &filename : configurationPaths) {
            this->logger->debug(
                    Glib::ustring::compose(
                            this->translator->trns("Found configuration on path: %1"),
                            filename
                    )
            );
            Glib::ustring container;
            std::ostringstream buffer;
            std::ifstream input(filename);
            if (!input.good()) {
                this->logger->debug(
                        Glib::ustring::compose(
                                this->translator->trns("Failed to load file: %1"),
                                filename
                        )
                );
                continue;
            }

            buffer << input.rdbuf();
            container = buffer.str();
            std::list<Device_ptr> justLoadedDevices = this->parser->parseDevices(container);

            this->resolver->mergeConfigurationOnTopOf(systemWideDevices, justLoadedDevices);
        }
    } else {
        this->logger->warning(this->translator->trns("System-wide configuration path doesn't exist!"));
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
        this->logger->debug(
                this->translator->trns("User defined configuration is empty. Returning an empty object")
        );
        std::list<Device_ptr> deviceList;
        return deviceList;
    }
    return this->parser->parseDevices(userDefinedXML);
}

Glib::ustring ConfigurationLoader::getOldSystemWideConfigurationPath() {
    Glib::ustring path(SYSCONFDIR "/drirc");
    std::ifstream flatpakInfo("/.flatpak-info");

    if (flatpakInfo.good()) {
        path = "/var/run/host" SYSCONFDIR "/drirc";
    }

    return path;
}

boost::filesystem::path ConfigurationLoader::getSystemWideConfigurationPath() {
    std::ifstream flatpakInfo("/.flatpak-info");

    if (flatpakInfo.good()) {
        return boost::filesystem::path("/var/run/host" DATADIR);
    }

    return boost::filesystem::path(DATADIR);
}

ConfigurationLoader::ConfigurationLoader(
        const DRIQuery &driQuery,
        DisplayServerQueryInterface *displayQuery,
        LoggerInterface *logger,
        TranslatorInterface *translator,
        ParserInterface *parser,
        ConfigurationResolverInterface *resolver
)
        : driQuery(driQuery),
          displayQuery(displayQuery),
          logger(logger),
          translator(translator),
          parser(parser),
          resolver(resolver) {}
