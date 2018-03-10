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

uint16_t GPUInfo::getVendorId() const {
    return vendorId;
}

void GPUInfo::setVendorId(uint16_t vendorId) {
    GPUInfo::vendorId = vendorId;
}

uint16_t GPUInfo::getDeviceId() const {
    return deviceId;
}

void GPUInfo::setDeviceId(uint16_t deviceId) {
    GPUInfo::deviceId = deviceId;
}

const std::list<Section> &GPUInfo::getSections() const {
    return sections;
}

void GPUInfo::setSections(const std::list<Section> &sections) {
    this->sections = sections;
}

bool GPUInfo::operator==(const GPUInfo &rhs) {
    return this->getDeviceId() == rhs.getDeviceId() && this->getVendorId() == rhs.getVendorId();
}

std::map<Glib::ustring, Glib::ustring> GPUInfo::getOptionsMap() {
    std::map<Glib::ustring, Glib::ustring> optionMap;

    for (const auto &section : this->sections) {
        for (const auto &option : section.getOptions()) {
            optionMap[option.getName()] = option.getDefaultValue();
        }
    }

    return optionMap;
}
