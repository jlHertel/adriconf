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
    for(const auto &section : this->sections) {
        for(const auto &option : section.getOptions()) {
            if(option.getName() == optionName) {
                return option.getEnumValues();
            }
        }
    }
}
