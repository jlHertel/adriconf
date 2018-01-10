#include "DriverConfiguration.h"

const Glib::ustring &DRI::DriverConfiguration::getDriver() const {
    return driver;
}

void DRI::DriverConfiguration::setDriver(Glib::ustring driver) {
    DriverConfiguration::driver = std::move(driver);
}

const int &DRI::DriverConfiguration::getScreen() const {
    return screen;
}

void DRI::DriverConfiguration::setScreen(int screen) {
    DriverConfiguration::screen = screen;
}

const std::list<DRI::Section> &DRI::DriverConfiguration::getSections() const {
    return sections;
}

void DRI::DriverConfiguration::setSections(const std::list<DRI::Section> &sections) {
    this->sections = sections;
}

std::list<std::pair<Glib::ustring, Glib::ustring>>
DRI::DriverConfiguration::getEnumValuesForOption(const Glib::ustring &optionName) {
    for (const auto &section : this->sections) {
        for (const auto &option : section.getOptions()) {
            if (option.getName() == optionName) {
                return option.getEnumValues();
            }
        }
    }
}

std::shared_ptr<DRI::Application> DRI::DriverConfiguration::generateApplication() const {
    std::shared_ptr<DRI::Application> app = std::make_shared<DRI::Application>();
    std::list<std::shared_ptr<DRI::ApplicationOption>> options;

    for (const auto &section : this->sections) {
        for (const auto &option : section.getOptions()) {
            auto driverOpt = std::make_shared<DRI::ApplicationOption>();
            driverOpt->setName(option.getName());
            driverOpt->setValue(option.getDefaultValue());

            options.emplace_back(driverOpt);
        }
    }

    app->setOptions(options);

    return app;
}
