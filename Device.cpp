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

std::list<std::shared_ptr<Application>> &Device::getApplications() {
    return this->applications;
}

const std::list<std::shared_ptr<Application>> &Device::getApplications() const {
    return this->applications;
}

void Device::addApplication(std::shared_ptr<Application> application) {
    this->applications.emplace_back(application);
}

Device::Device() : driver(""), screen(-1), applications() {}

std::shared_ptr<Application> Device::findApplication(const Glib::ustring &executable) const {
    for (auto app : this->applications) {
        if (app->getExecutable() == executable) {
            return app;
        }
    }

    return nullptr;
}

void Device::sortApplications() {
    this->applications.sort([](std::shared_ptr<Application> a, std::shared_ptr<Application> b) {
        return a->getName() < b->getName();
    });
}