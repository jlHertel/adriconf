#include "Application.h"

const Glib::ustring &Application::getName() const {
    return name;
}

void Application::setName(Glib::ustring name) {
    this->name = std::move(name);
}

const Glib::ustring &Application::getExecutable() const {
    return executable;
}

void Application::setExecutable(Glib::ustring executable) {
    this->executable = std::move(executable);
}

std::list<ApplicationOption_ptr> &Application::getOptions() {
    return this->options;
}

void Application::addOption(ApplicationOption_ptr option) {
    this->options.emplace_back(option);
}

void Application::setOptions(std::list<ApplicationOption_ptr> options) {
    this->options = std::move(options);
}

bool Application::getIsUsingPrime() const {
    return isUsingPrime;
}

void Application::setIsUsingPrime(bool isUsingPrime) {
    Application::isUsingPrime = isUsingPrime;
}

const Glib::ustring &Application::getPrimeDriverName() const {
    return primeDriverName;
}

void Application::setPrimeDriverName(const Glib::ustring &primeDriverName) {
    Application::primeDriverName = primeDriverName;
}

const Glib::ustring &Application::getDevicePCIId() const {
    return devicePCIId;
}

void Application::setDevicePCIId(const Glib::ustring &devicePCIId) {
    Application::devicePCIId = devicePCIId;
}

std::map<Glib::ustring, Glib::ustring> Application::getOptionsAsMap() {
    std::map<Glib::ustring, Glib::ustring> optionMap;

    for(auto & option : this->options) {
        optionMap[option->getName()] = option->getValue();
    }

    return optionMap;
}
