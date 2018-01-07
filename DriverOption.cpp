#include "DriverOption.h"

#include <utility>


const Glib::ustring &DRI::DriverOption::getName() const {
    return this->name;
}

const Glib::ustring &DRI::DriverOption::getDescription() const {
    return this->description;
}

const Glib::ustring &DRI::DriverOption::getType() const {
    return this->type;
}

const Glib::ustring &DRI::DriverOption::getDefaultValue() const {
    return this->defaultValue;
}

const Glib::ustring &DRI::DriverOption::getValidValues() const {
    return this->validValues;
}

std::list<std::pair<Glib::ustring, Glib::ustring>> DRI::DriverOption::getEnumValues() const{
    return this->enumValues;
}


DRI::DriverOption *DRI::DriverOption::setName(Glib::ustring name) {
    this->name = std::move(name);

    return this;
}

DRI::DriverOption *DRI::DriverOption::setDescription(Glib::ustring description) {
    this->description = std::move(description);

    return this;
}

DRI::DriverOption *DRI::DriverOption::setType(Glib::ustring type) {
    this->type = std::move(type);

    return this;
}

DRI::DriverOption *DRI::DriverOption::setDefaultValue(Glib::ustring defaultValue) {
    this->defaultValue = std::move(defaultValue);

    return this;
}

DRI::DriverOption *DRI::DriverOption::setValidValues(Glib::ustring validValues) {
    this->validValues = std::move(validValues);

    return this;
}

DRI::DriverOption *DRI::DriverOption::addEnumValue(Glib::ustring description, Glib::ustring value) {
    this->enumValues.emplace_back(description, value);

    return this;
}

int DRI::DriverOption::getValidValueStart() const {
    if(this->validValues.empty()) {
        return 0;
    }

    auto splitPos = this->validValues.find_first_of(':');
    if(splitPos > this->validValues.length() || splitPos <= 0) {
        return 0;
    }

    // The first part up to the new line
    auto firstPart = this->validValues.substr(0, splitPos);

    return std::stoi(firstPart);
}

int DRI::DriverOption::getValidValueEnd() const {
    if(this->validValues.empty()) {
        return 10000;
    }

    auto splitPos = this->validValues.find_first_of(':');
    if(splitPos > this->validValues.length() || splitPos <= 0) {
        return 10000;
    }

    // The part after the new line
    auto secondPart = this->validValues.substr(splitPos + 1, this->validValues.length() - splitPos);

    return std::stoi(secondPart);
}
