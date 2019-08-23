#ifndef ADRICONF_PCIDATABASEQUERY_H
#define ADRICONF_PCIDATABASEQUERY_H

#include <glibmm/ustring.h>
#include "PCIDatabaseQueryInterface.h"

extern "C" {
#include <pci/pci.h>
}

class PCIDatabaseQuery : public PCIDatabaseQueryInterface {
private:
    struct pci_access *pci;

public:
    PCIDatabaseQuery();

    ~PCIDatabaseQuery() override;

    Glib::ustring queryVendorName(uint16_t vendorId) override;
    Glib::ustring queryDeviceName(uint16_t vendorId, uint16_t deviceId) override;
};


#endif //ADRICONF_PCIDATABASEQUERY_H
