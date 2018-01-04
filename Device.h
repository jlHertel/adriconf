#ifndef DRICONF3_DEVICE_H
#define DRICONF3_DEVICE_H

#include <list>
#include <glibmm/ustring.h>
#include "Application.h"

namespace DRI {
    class Device {
    private:
        Glib::ustring driver;
        int screen;
        std::list<DRI::Application> applications;

    public:
        Glib::ustring getDriver() const;

        void setDriver(const Glib::ustring &driver);

        int getScreen() const;

        void setScreen(int screen);

        std::list<DRI::Application> getApplications() const;

        void addApplication(DRI::Application application);

        void setApplications(std::list<DRI::Application> applications);

        DRI::Application findApplication(Glib::ustring executable) const;

        bool applicationExists(Glib::ustring executable) const;

        Device(const Glib::ustring &driver, int screen);

        Device();
    };
}

#endif
