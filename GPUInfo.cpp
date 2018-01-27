#include "GPUInfo.h"

const Glib::ustring &GPUInfo::getPciId() const {
    return pciId;
}

void GPUInfo::setPciId(const Glib::ustring &pciId) {
    GPUInfo::pciId = pciId;
}

const Glib::ustring &GPUInfo::getDriverName() const {
    return driverName;
}

void GPUInfo::setDriverName(const Glib::ustring &driverName) {
    GPUInfo::driverName = driverName;
}

const Glib::ustring &GPUInfo::getDeviceName() const {
    return deviceName;
}

void GPUInfo::setDeviceName(const Glib::ustring &deviceName) {
    GPUInfo::deviceName = deviceName;
}

const Glib::ustring &GPUInfo::getVendorName() const {
    return vendorName;
}

void GPUInfo::setVendorName(const Glib::ustring &vendorName) {
    GPUInfo::vendorName = vendorName;
}
