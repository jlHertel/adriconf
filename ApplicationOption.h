#ifndef ADRICONF_APPLICATIONOPTION_H
#define ADRICONF_APPLICATIONOPTION_H

#include <glibmm/ustring.h>
#include <memory>

class ApplicationOption {
private:
    Glib::ustring name;
    Glib::ustring value;

public:
    const Glib::ustring &getName() const;

    void setName(Glib::ustring name);

    const Glib::ustring &getValue() const;

    void setValue(Glib::ustring value);
};

typedef std::shared_ptr<ApplicationOption> ApplicationOption_ptr;

#endif
