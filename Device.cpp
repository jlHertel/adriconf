#include "Device.h"

#include <algorithm>

const Glib::ustring &Device::getDriver() const {
    return this->driver;
}

void Device::setDriver(Glib::ustring driver) {
    this->driver = std::move(driver);
}

const int &Device::getScreen() const {
    return this->screen;
}

void Device::setScreen(int screen) {
    this->screen = screen;
}

std::list<Application_ptr> &Device::getApplications() {
    return this->applications;
}

const std::list<Application_ptr> &Device::getApplications() const {
    return this->applications;
}

void Device::addApplication(Application_ptr application) {
    this->applications.emplace_back(application);
}

Device::Device() : driver(""), screen(-1), applications() {}

Application_ptr Device::findApplication(const Glib::ustring &executable) const {
    for (auto app : this->applications) {
        if (app->getExecutable() == executable) {
            return app;
        }
    }

    return nullptr;
}

void Device::sortApplications() {
    this->applications.sort([](Application_ptr a, Application_ptr b) {
        return a->getName() < b->getName();
    });
}