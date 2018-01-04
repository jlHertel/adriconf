#ifndef DRICONF3_DRIVERCONFIGURATION_H
#define DRICONF3_DRIVERCONFIGURATION_H

#include <list>
#include <glibmm/ustring.h>
#include "Application.h"
#include "Section.h"

namespace DRI {
    class DriverConfiguration {
    private:
        Glib::ustring driver;
        int screen;
        std::list<DRI::Section> sections;

    public:
        const Glib::ustring &getDriver() const;

        void setDriver(const Glib::ustring &driver);

        int getScreen() const;

        void setScreen(int screen);

        const std::list<Section> &getSections() const;

        void setSections(const std::list<Section> &sections);
    };
};

#endif
