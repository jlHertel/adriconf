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

std::list<DRI::Application *> &DRI::Device::getApplications() {
    return this->applications;
}

const std::list<DRI::Application *> &DRI::Device::getApplications() const {
    return this->applications;
}

void DRI::Device::addApplication(DRI::Application *application) {
    this->applications.emplace_back(application);
}

DRI::Device::Device(Glib::ustring driver, int screen) : driver(std::move(driver)), screen(screen), applications() {}

DRI::Device::Device() : driver(""), screen(-1), applications() {}

DRI::Application *DRI::Device::findApplication(const Glib::ustring &executable) const {
    for (auto app : this->applications) {
        if (app->getExecutable() == executable) {
            return app;
        }
    }

    return nullptr;
}

void DRI::Device::sortApplications() {
    this->applications.sort([](DRI::Application *a, DRI::Application *b) {
        return a->getName() < b->getName();
    });
}

DRI::Device::~Device() {
    for (auto &&app : this->applications) {
        delete app;
    }
}

DRI::Device::Device(const DRI::Device &copy) {
    this->driver = copy.getDriver();
    this->screen = copy.getScreen();

}
