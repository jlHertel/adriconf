#ifndef DRICONF3_OPTION_H
#define DRICONF3_OPTION_H

#include <iostream>
#include <glibmm/ustring.h>
#include <list>

namespace DRI {
    class Option {
    private:
        Glib::ustring name;
        Glib::ustring description;
        Glib::ustring type;
        Glib::ustring defaultValue;
        Glib::ustring validValues;
        std::list<std::pair<Glib::ustring, Glib::ustring>> enumValues;

    public:
        Glib::ustring getName();

        Glib::ustring getDescription();

        Glib::ustring getType();

        Glib::ustring getDefaultValue();

        Glib::ustring getValidValues();

        std::list<std::pair<Glib::ustring, Glib::ustring>> getEnumValues();

        Option *setName(Glib::ustring name);

        Option *setDescription(Glib::ustring description);

        Option *setType(Glib::ustring type);

        Option *setDefaultValue(Glib::ustring defaultValue);

        Option *setValidValues(Glib::ustring validValues);

        Option *addEnumValue(Glib::ustring description, Glib::ustring value);


    };
}

#endif //DRICONF3_OPTION_H
