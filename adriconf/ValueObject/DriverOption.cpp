#include "DriverOption.h"

#include <utility>


const Glib::ustring &DriverOption::getName() const {
    return this->name;
}

const Glib::ustring &DriverOption::getDescription() const {
    return this->description;
}

const DriverOptionType &DriverOption::getType() const {
    return this->type;
}

void DriverOption::updateFakeBool() {
    if (this->type == DriverOptionType::ENUM
        && this->validValues == "0:1"
        && this->enumValues.empty()) {
        this->type = DriverOptionType::FAKE_BOOL;
    }
}

DriverOptionType DriverOption::stringToEnum(const Glib::ustring &type) const {
    if (type == "bool") {
        return DriverOptionType::BOOL;
    }

    if (type == "int") {
        return DriverOptionType::INT;
    }

    if (type == "enum") {
        return DriverOptionType::ENUM;
    }

    return DriverOptionType::UNKNOW;
}

const Glib::ustring &DriverOption::getDefaultValue() const {
    return this->defaultValue;
}

const Glib::ustring &DriverOption::getValidValues() const {
    return this->validValues;
}

std::list<std::pair<Glib::ustring, Glib::ustring>>
DriverOption::getEnumValues() const {
    return this->enumValues;
}


DriverOption *DriverOption::setName(Glib::ustring name) {
    this->name = std::move(name);

    return this;
}

DriverOption *DriverOption::setDescription(Glib::ustring description) {
    this->description = std::move(description);

    return this;
}

DriverOption *DriverOption::setType(DriverOptionType type) {
    this->type = type;

    return this;
}

DriverOption *DriverOption::setDefaultValue(Glib::ustring defaultValue) {
    this->defaultValue = std::move(defaultValue);

    return this;
}

DriverOption *DriverOption::setValidValues(Glib::ustring validValues) {
    this->validValues = std::move(validValues);

    return this;
}

DriverOption *DriverOption::addEnumValue(
        Glib::ustring description, Glib::ustring value
) {
    this->enumValues.emplace_back(description, value);

    return this;
}

int DriverOption::getValidValueStart() const {
    if (this->validValues.empty()) {
        return -1;
    }

    auto splitPos = this->validValues.find_first_of(':');
    if (splitPos > this->validValues.length() || splitPos <= 0) {
        return -1;
    }

    // The first part up to the new line
    auto firstPart = this->validValues.substr(0, splitPos);

    return std::stoi(firstPart);
}

int DriverOption::getValidValueEnd() const {
    if (this->validValues.empty()) {
        return 10000;
    }

    auto splitPos = this->validValues.find_first_of(':');
    if (splitPos == std::string::npos || splitPos >= this->validValues.length() - 1) {
        return 10000;
    }

    // The part after the new line
    auto secondPart = this->validValues.substr(splitPos + 1,
                                               this->validValues.length() -
                                               splitPos);

    return std::stoi(secondPart);
}

int DriverOption::getSortValue() const {
    switch (this->type) {
        case DriverOptionType::BOOL:
        case DriverOptionType::FAKE_BOOL:
            return 1;

        case DriverOptionType::ENUM:
            return 2;

        case DriverOptionType::INT:
            return 3;

        default:
            return 4;
    }
}

