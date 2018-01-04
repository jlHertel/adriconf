#ifndef DRICONF3_APPLICATION_H
#define DRICONF3_APPLICATION_H

#include <glibmm/ustring.h>
#include <list>
#include <map>

namespace DRI {
    class Application {
    private:
        Glib::ustring name;
        Glib::ustring executable;
        /* name-value of the options */
        std::map<Glib::ustring, Glib::ustring> options;

    public:
        const Glib::ustring getName() const;

        void setName(const Glib::ustring &name);

        const Glib::ustring getExecutable() const;

        void setExecutable(const Glib::ustring &executable);

        const std::map<Glib::ustring, Glib::ustring> getOptions() const;

        void addOption(Glib::ustring name, Glib::ustring value);

        void setOptions(std::map<Glib::ustring, Glib::ustring> options);
    };
};

#endif
