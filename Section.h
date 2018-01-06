#ifndef DRICONF3_SECTION_H
#define DRICONF3_SECTION_H

#include <glibmm/ustring.h>
#include <list>
#include "DriverOption.h"

namespace DRI {
    class Section {
    private:
        Glib::ustring description;
        std::list<DRI::DriverOption> options;

    public:
        Section();

        const Glib::ustring &getDescription() const;

        const std::list<DRI::DriverOption> &getOptions() const;

        Section *setDescription(Glib::ustring description);

        Section *addOption(DRI::DriverOption option);
    };
}

#endif