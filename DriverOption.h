#ifndef DRICONF3_OPTION_H
#define DRICONF3_OPTION_H

#include <iostream>
#include <glibmm/ustring.h>
#include <list>

namespace DRI {
    class DriverOption {
    private:
        Glib::ustring name;
        Glib::ustring description;
        Glib::ustring type;
        Glib::ustring defaultValue;
        Glib::ustring validValues;
        std::list<std::pair<Glib::ustring, Glib::ustring>> enumValues;

    public:
        const Glib::ustring &getName() const;

        const Glib::ustring &getDescription() const;

        const Glib::ustring &getType() const;

        const Glib::ustring &getDefaultValue() const;

        const Glib::ustring &getValidValues() const;

        int getValidValueStart() const;

        int getValidValueEnd() const;

        std::list<std::pair<Glib::ustring, Glib::ustring>> getEnumValues() const;

        DriverOption *setName(Glib::ustring name);

        DriverOption *setDescription(Glib::ustring description);

        DriverOption *setType(Glib::ustring type);

        DriverOption *setDefaultValue(Glib::ustring defaultValue);

        DriverOption *setValidValues(Glib::ustring validValues);

        DriverOption *addEnumValue(Glib::ustring description, Glib::ustring value);
    };
}

#endif //DRICONF3_OPTION_H
