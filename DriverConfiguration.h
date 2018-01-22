#ifndef DRICONF3_DRIVERCONFIGURATION_H
#define DRICONF3_DRIVERCONFIGURATION_H

#include <list>
#include <glibmm/ustring.h>
#include "Application.h"
#include "Section.h"

class DriverConfiguration {
private:
    Glib::ustring driver;
    int screen;
    std::list<Section> sections;

public:
    const Glib::ustring &getDriver() const;

    void setDriver(Glib::ustring driver);

    const int &getScreen() const;

    void setScreen(int screen);

    const std::list<Section> &getSections() const;

    void setSections(const std::list<Section> &sections);

    std::list<std::pair<Glib::ustring, Glib::ustring>> getEnumValuesForOption(const Glib::ustring &);

    /* Generate a new application based on this driver-supported options */
    Application_ptr generateApplication() const;

    /* Sort the options inside each section to be more user-friendly */
    void sortSectionOptions();
};

#endif
