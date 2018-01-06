#include "ApplicationOption.h"

const Glib::ustring &DRI::ApplicationOption::getName() const {
    return name;
}

void DRI::ApplicationOption::setName(Glib::ustring name) {
    ApplicationOption::name = std::move(name);
}

const Glib::ustring &DRI::ApplicationOption::getValue() const {
    return value;
}

void DRI::ApplicationOption::setValue(Glib::ustring value) {
    ApplicationOption::value = std::move(value);
}
