#include "Application.h"

const Glib::ustring &DRI::Application::getName() const {
    return name;
}

void DRI::Application::setName(Glib::ustring name) {
    this->name = std::move(name);
}

const Glib::ustring &DRI::Application::getExecutable() const {
    return executable;
}

void DRI::Application::setExecutable(Glib::ustring executable) {
    this->executable = std::move(executable);
}

std::list<DRI::ApplicationOption *> &DRI::Application::getOptions() {
    return this->options;
}

void DRI::Application::addOption(DRI::ApplicationOption *option) {
    this->options.emplace_back(option);
}

DRI::Application::~Application() {
    for(auto &option : this->options) {
        delete option;
    }
}
