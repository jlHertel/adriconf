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

std::shared_ptr<Application> DriverConfiguration::generateApplication() const {
    std::shared_ptr<Application> app = std::make_shared<Application>();
    std::list<std::shared_ptr<ApplicationOption>> options;

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
