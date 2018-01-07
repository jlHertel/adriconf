#include "Device.h"

#include <algorithm>

const Glib::ustring &DRI::Device::getDriver() const {
    return this->driver;
}

void DRI::Device::setDriver(Glib::ustring driver) {
    this->driver = std::move(driver);
}

const int &DRI::Device::getScreen() const {
    return this->screen;
}

void DRI::Device::setScreen(int screen) {
    this->screen = screen;
}

std::list<std::shared_ptr<DRI::Application>> &DRI::Device::getApplications() {
    return this->applications;
}

const std::list<std::shared_ptr<DRI::Application>> &DRI::Device::getApplications() const {
    return this->applications;
}

void DRI::Device::addApplication(std::shared_ptr<DRI::Application> application) {
    this->applications.emplace_back(application);
}

DRI::Device::Device() : driver(""), screen(-1), applications() {}

std::shared_ptr<DRI::Application> DRI::Device::findApplication(const Glib::ustring &executable) const {
    for (auto app : this->applications) {
        if (app->getExecutable() == executable) {
            return app;
        }
    }

    return nullptr;
}

void DRI::Device::sortApplications() {
    this->applications.sort([](std::shared_ptr<DRI::Application> a, std::shared_ptr<DRI::Application> b) {
        return a->getName() < b->getName();
    });
}