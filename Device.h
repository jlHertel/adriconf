#ifndef DRICONF3_DEVICE_H
#define DRICONF3_DEVICE_H

#include <memory>
#include <list>
#include <glibmm/ustring.h>
#include "Application.h"

class Device {
private:
    Glib::ustring driver;
    int screen;
    std::list<Application_ptr> applications;

public:
    const Glib::ustring &getDriver() const;

    void setDriver(Glib::ustring driver);

    const int &getScreen() const;

    void setScreen(int screen);

    std::list<Application_ptr> &getApplications();

    const std::list<Application_ptr> &getApplications() const;

    void addApplication(Application_ptr application);

    Application_ptr findApplication(const Glib::ustring &executable) const;

    void sortApplications();

    Device();
};

typedef std::shared_ptr<Device> Device_ptr;

#endif
