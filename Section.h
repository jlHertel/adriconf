#ifndef DRICONF3_SECTION_H
#define DRICONF3_SECTION_H

#include <glibmm/ustring.h>
#include <list>
#include "Option.h"

namespace DRI {
    class Section {
    private:
        Glib::ustring description;
        std::list<DRI::Option> options;

    public:
        Section();

        Glib::ustring getDescription();

        std::list<DRI::Option> getOptions();

        Section *setDescription(Glib::ustring description);

        Section *addOption(DRI::Option option);
    };
}

#endif