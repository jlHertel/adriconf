#include "Device.h"

#include <algorithm>

Glib::ustring DRI::Device::getDriver() const {
    return this->driver;
}

void DRI::Device::setDriver(const Glib::ustring &driver) {
    this->driver = driver;
}

int DRI::Device::getScreen() const {
    return this->screen;
}

void DRI::Device::setScreen(int screen) {
    this->screen = screen;
}

std::list<DRI::Application> DRI::Device::getApplications() const {
    return this->applications;
}

void DRI::Device::addApplication(DRI::Application application) {
    this->applications.emplace_back(application);
}

void DRI::Device::setApplications(std::list<DRI::Application> applications) {
    this->applications = std::move(applications);
}

DRI::Device::Device(const Glib::ustring &driver, int screen) : driver(driver), screen(screen), applications() {}

DRI::Device::Device() : driver(""), screen(-1), applications() {}

DRI::Application DRI::Device::findApplication(Glib::ustring executable) const {
    for (auto app : this->getApplications()) {
        if (app.getExecutable() == executable) {
            return app;
        }
    }

    /* This should be unreachable as you should always check first if the app exists */
    DRI::Application fakeApp;
    return fakeApp;
}

bool DRI::Device::applicationExists(Glib::ustring executable) const {
    for (const auto &app : this->getApplications()) {
        if (app.getExecutable() == executable) {
            return true;
        }
    }

    return false;
}

void DRI::Device::sortApplications() {
    this->applications.sort([](DRI::Application a, DRI::Application b) {
        return a.getName() < b.getName();
    });
}
