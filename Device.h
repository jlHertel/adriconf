#ifndef DRICONF3_DEVICE_H
#define DRICONF3_DEVICE_H

#include <memory>
#include <list>
#include <glibmm/ustring.h>
#include "Application.h"

namespace DRI {
    class Device {
    private:
        Glib::ustring driver;
        int screen;
        std::list<std::shared_ptr<DRI::Application>> applications;

    public:
        const Glib::ustring &getDriver() const;

        void setDriver(Glib::ustring driver);

        const int &getScreen() const;

        void setScreen(int screen);

        std::list<std::shared_ptr<DRI::Application>> &getApplications();

        const std::list<std::shared_ptr<DRI::Application>> &getApplications() const;

        void addApplication(std::shared_ptr<DRI::Application> application);

        std::shared_ptr<DRI::Application> findApplication(const Glib::ustring &executable) const;

        void sortApplications();

        Device();
    };
}

#endif
