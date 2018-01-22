#ifndef DRICONF3_SECTION_H
#define DRICONF3_SECTION_H

#include <glibmm/ustring.h>
#include <list>
#include "DriverOption.h"

class Section {
private:
    Glib::ustring description;
    std::list<DriverOption> options;

public:
    Section();

    const Glib::ustring &getDescription() const;

    const std::list<DriverOption> &getOptions() const;

    Section *setDescription(Glib::ustring description);

    Section *addOption(DriverOption option);

    /* Sort the options of this section */
    void sortOptions();
};

#endif