#include "ApplicationOption.h"

const Glib::ustring &ApplicationOption::getName() const {
    return name;
}

void ApplicationOption::setName(Glib::ustring name) {
    ApplicationOption::name = std::move(name);
}

const Glib::ustring &ApplicationOption::getValue() const {
    return value;
}

void ApplicationOption::setValue(Glib::ustring value) {
    ApplicationOption::value = std::move(value);
}
