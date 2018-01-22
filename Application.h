#ifndef DRICONF3_APPLICATION_H
#define DRICONF3_APPLICATION_H

#include <glibmm/ustring.h>
#include <list>
#include "ApplicationOption.h"
#include <memory>

class Application {
private:
    Glib::ustring name;
    Glib::ustring executable;
    std::list<ApplicationOption_ptr> options;

public:
    const Glib::ustring &getName() const;

    void setName(Glib::ustring name);

    const Glib::ustring &getExecutable() const;

    void setExecutable(Glib::ustring executable);

    std::list<ApplicationOption_ptr> &getOptions();

    void addOption(ApplicationOption_ptr option);

    void setOptions(std::list<ApplicationOption_ptr>);
};

typedef std::shared_ptr<Application> Application_ptr;

#endif
