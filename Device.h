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
        std::list<DRI::Application*> applications;

    public:
        const Glib::ustring &getDriver() const;

        void setDriver(Glib::ustring driver);

        const int &getScreen() const;

        void setScreen(int screen);

        std::list<DRI::Application*> &getApplications();

        const std::list<DRI::Application*> &getApplications() const;

        void addApplication(DRI::Application* application);

        DRI::Application* findApplication(const Glib::ustring &executable) const;

        void sortApplications();

        Device(Glib::ustring driver, int screen);

        Device();

        Device(const Device&);

        virtual ~Device();
    };
}

#endif
