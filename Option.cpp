#include "Option.h"

#include <utility>


Glib::ustring DRI::Option::getName() {
    return this->name;
}

Glib::ustring DRI::Option::getDescription() {
    return this->description;
}

Glib::ustring DRI::Option::getType() {
    return this->type;
}

Glib::ustring DRI::Option::getDefaultValue() {
    return this->defaultValue;
}

Glib::ustring DRI::Option::getValidValues() {
    return this->validValues;
}

std::list<std::pair<Glib::ustring, Glib::ustring>> DRI::Option::getEnumValues() {
    return this->enumValues;
}


DRI::Option *DRI::Option::setName(Glib::ustring name) {
    this->name = std::move(name);

    return this;
}

DRI::Option *DRI::Option::setDescription(Glib::ustring description) {
    this->description = std::move(description);

    return this;
}

DRI::Option *DRI::Option::setType(Glib::ustring type) {
    this->type = std::move(type);

    return this;
}

DRI::Option *DRI::Option::setDefaultValue(Glib::ustring defaultValue) {
    this->defaultValue = std::move(defaultValue);

    return this;
}

DRI::Option *DRI::Option::setValidValues(Glib::ustring validValues) {
    this->validValues = std::move(validValues);

    return this;
}

DRI::Option *DRI::Option::addEnumValue(Glib::ustring description, Glib::ustring value) {
    this->enumValues.emplace_back(description, value);

    return this;
}
