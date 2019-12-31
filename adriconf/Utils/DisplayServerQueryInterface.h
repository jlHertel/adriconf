#ifndef ADRICONF_DISPLAYSERVERQUERYINTERFACE_H
#define ADRICONF_DISPLAYSERVERQUERYINTERFACE_H

#include <list>
#include "../ValueObject/DriverConfiguration.h"
#include <glibmm/ustring.h>

class DisplayServerQueryInterface {
public:
    virtual std::list<DriverConfiguration> queryDriverConfigurationOptions(const Glib::ustring &locale) = 0;
    virtual bool checkNecessaryExtensions() = 0;
};

#endif //ADRICONF_DISPLAYSERVERQUERYINTERFACE_H