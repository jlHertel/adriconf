#include "Application.h"

const Glib::ustring DRI::Application::getName() const {
    return name;
}

void DRI::Application::setName(const Glib::ustring &name) {
    this->name = name;
}

const Glib::ustring DRI::Application::getExecutable() const {
    return executable;
}

void DRI::Application::setExecutable(const Glib::ustring &executable) {
    this->executable = executable;
}

const std::map<Glib::ustring, Glib::ustring> DRI::Application::getOptions() const {
    return options;
}

void DRI::Application::addOption(Glib::ustring name, Glib::ustring value) {
    this->options.emplace(name, value);
}

void DRI::Application::setOptions(std::map<Glib::ustring, Glib::ustring> options) {
    this->options = std::move(options);
}
