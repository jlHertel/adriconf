#ifndef ADRICONF_PCIDATABASEQUERY_H
#define ADRICONF_PCIDATABASEQUERY_H

#include <glibmm/ustring.h>

extern "C" {
#include <pci/pci.h>
}

class PCIDatabaseQuery {
private:
    struct pci_access *pci;

public:
    PCIDatabaseQuery();

    virtual ~PCIDatabaseQuery();

    Glib::ustring queryVendorName(uint16_t);
    Glib::ustring queryDeviceName(uint16_t, uint16_t);
};


#endif //ADRICONF_PCIDATABASEQUERY_H
