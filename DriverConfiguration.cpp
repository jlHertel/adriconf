#include "DriverConfiguration.h"

const Glib::ustring &DriverConfiguration::getDriver() const {
    return driver;
}

void DriverConfiguration::setDriver(Glib::ustring driver) {
    DriverConfiguration::driver = std::move(driver);
}

const int &DriverConfiguration::getScreen() const {
    return screen;
}

void DriverConfiguration::setScreen(int screen) {
    DriverConfiguration::screen = screen;
}

const std::list<Section> &DriverConfiguration::getSections() const {
    return sections;
}

void DriverConfiguration::setSections(const std::list<Section> &sections) {
    this->sections = sections;
}

std::list<std::pair<Glib::ustring, Glib::ustring>>
DriverConfiguration::getEnumValuesForOption(const Glib::ustring &optionName) {
    for (const auto &section : this->sections) {
        for (const auto &option : section.getOptions()) {
            if (option.getName() == optionName) {
                return option.getEnumValues();
            }
        }
    }
}

Application_ptr DriverConfiguration::generateApplication() const {
    Application_ptr app = std::make_shared<Application>();
    std::list<ApplicationOption_ptr> options;

    for (const auto &section : this->sections) {
        for (const auto &option : section.getOptions()) {
            auto driverOpt = std::make_shared<ApplicationOption>();
            driverOpt->setName(option.getName());
            driverOpt->setValue(option.getDefaultValue());

            options.emplace_back(driverOpt);
        }
    }

    app->setOptions(options);

    return app;
}

void DriverConfiguration::sortSectionOptions() {
    for (auto &section : this->sections) {
        section.sortOptions();
    }
}

uint16_t DriverConfiguration::getVendorId() const {
    return vendorId;
}

void DriverConfiguration::setVendorId(uint16_t vendorId) {
    DriverConfiguration::vendorId = vendorId;
}

uint16_t DriverConfiguration::getDeviceId() const {
    return deviceId;
}

void DriverConfiguration::setDeviceId(uint16_t deviceId) {
    DriverConfiguration::deviceId = deviceId;
}

std::map<Glib::ustring, Glib::ustring> DriverConfiguration::getOptionsMap() {
    std::map<Glib::ustring, Glib::ustring> optionMap;

    for (const auto &section : this->sections) {
        for (const auto &option : section.getOptions()) {
            optionMap[option.getName()] = option.getDefaultValue();
        }
    }

    return optionMap;
}
