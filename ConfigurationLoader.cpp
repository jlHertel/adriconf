#include "ConfigurationLoader.h"

#include <fstream>
#include "DRIQuery.h"

Glib::ustring DRI::ConfigurationLoader::readSystemWideXML() {
    std::ostringstream buffer;
    std::ifstream input("/etc/drirc");
    buffer << input.rdbuf();
    Glib::ustring container(buffer.str());

    return std::move(container);
}

Glib::ustring DRI::ConfigurationLoader::readUserDefinedXML() {
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

std::list<DRI::DriverConfiguration> DRI::ConfigurationLoader::loadDriverSpecificConfiguration(const Glib::ustring &locale) {
    DRI::DRIQuery query;
    return query.queryDriverConfigurationOptions(locale);
}

std::shared_ptr<DRI::Device> DRI::ConfigurationLoader::loadSystemWideConfiguration() {
    Glib::ustring systemWideXML = this->readSystemWideXML();
    std::list<std::shared_ptr<DRI::Device>> systemWideDevices = DRI::Parser::parseDevices(systemWideXML);

    /* In case no configuration is available system-wide we generate an empty one */
    if(systemWideDevices.empty()) {
        auto fakeDevice = std::make_shared<DRI::Device>();
        systemWideDevices.emplace_back(fakeDevice);
    }

    return systemWideDevices.front();
}

std::list<std::shared_ptr<DRI::Device>> DRI::ConfigurationLoader::loadUserDefinedConfiguration() {
    Glib::ustring userDefinedXML(this->readUserDefinedXML());
    return DRI::Parser::parseDevices(userDefinedXML);
}
