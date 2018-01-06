#ifndef DRICONF3_APPLICATION_H
#define DRICONF3_APPLICATION_H

#include <glibmm/ustring.h>
#include <list>
#include "ApplicationOption.h"

namespace DRI {
    class Application {
    private:
        Glib::ustring name;
        Glib::ustring executable;
        std::list<DRI::ApplicationOption *> options;

    public:
        const Glib::ustring &getName() const;

        void setName(Glib::ustring name);

        const Glib::ustring &getExecutable() const;

        void setExecutable(Glib::ustring executable);

        std::list<DRI::ApplicationOption *> &getOptions();

        void addOption(DRI::ApplicationOption *option);

        virtual ~Application();
    };
};

#endif
