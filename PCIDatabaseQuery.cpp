#include "PCIDatabaseQuery.h"

PCIDatabaseQuery::PCIDatabaseQuery() {
    this->pci = pci_alloc();
    pci_init(this->pci);
}

PCIDatabaseQuery::~PCIDatabaseQuery() {
    pci_cleanup(this->pci);
}

Glib::ustring PCIDatabaseQuery::queryVendorName(uint16_t vendorId) {
    char buffer[1024], *lookepUpName;

    lookepUpName = pci_lookup_name(this->pci, buffer, sizeof(buffer), PCI_LOOKUP_VENDOR, vendorId);

    return Glib::ustring(lookepUpName);
}

Glib::ustring PCIDatabaseQuery::queryDeviceName(uint16_t vendorId, uint16_t deviceId) {
    char buffer[1024], *lookepUpName;

    lookepUpName = pci_lookup_name(this->pci, buffer, sizeof(buffer), PCI_LOOKUP_DEVICE, vendorId, deviceId);

    return Glib::ustring(lookepUpName);
}
