#include "DRMDevice.h"

#include <iomanip>

DRMDevice::DRMDevice(drmDevicePtr rawDevice) : rawDevice(rawDevice) {}

DRMDevice::~DRMDevice() {
    drmFreeDevice(&(this->rawDevice));
}

Glib::ustring DRMDevice::getFormattedPCIId() {
    return Glib::ustring::compose(
            "pci-%1_%2_%3_%4",
            Glib::ustring::format(std::setfill(L'0'), std::setw(4), std::hex,
                                  this->rawDevice->businfo.pci->domain),
            Glib::ustring::format(std::setfill(L'0'), std::setw(2), std::hex,
                                  this->rawDevice->businfo.pci->bus),
            Glib::ustring::format(std::setfill(L'0'), std::setw(2), std::hex,
                                  this->rawDevice->businfo.pci->dev),
            this->rawDevice->businfo.pci->func
    );
}

uint16_t DRMDevice::getVendorPCIId() {
    return this->rawDevice->deviceinfo.pci->vendor_id;
}

uint16_t DRMDevice::getDevicePCIId() {
    return this->rawDevice->deviceinfo.pci->device_id;
}

const char *DRMDevice::getDeviceRenderNodeName() {
    return this->rawDevice->nodes[DRM_NODE_RENDER];
}
