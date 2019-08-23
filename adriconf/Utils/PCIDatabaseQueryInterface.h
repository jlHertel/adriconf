#ifndef ADRICONF_PCIDATABASEQUERYINTERFACE_H
#define ADRICONF_PCIDATABASEQUERYINTERFACE_H

#include <glibmm/ustring.h>

class PCIDatabaseQueryInterface {
public:
    virtual ~PCIDatabaseQueryInterface() = default;

    virtual Glib::ustring queryVendorName(uint16_t vendorId) = 0;
    virtual Glib::ustring queryDeviceName(uint16_t vendorId, uint16_t deviceId) = 0;
};

#endif //ADRICONF_PCIDATABASEQUERYINTERFACE_H