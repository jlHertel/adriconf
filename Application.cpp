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

std::list<std::shared_ptr<DRI::ApplicationOption>> &DRI::Application::getOptions() {
    return this->options;
}

void DRI::Application::addOption(std::shared_ptr<DRI::ApplicationOption> option) {
    this->options.emplace_back(option);
}

void DRI::Application::setOptions(std::list<std::shared_ptr<DRI::ApplicationOption>> options) {
    this->options = std::move(options);
}