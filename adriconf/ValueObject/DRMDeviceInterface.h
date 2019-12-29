#ifndef ADRICONF_DRMDEVICEINTERFACE_H
#define ADRICONF_DRMDEVICEINTERFACE_H

#include <glibmm/ustring.h>
#include <cstdint>

class DRMDeviceInterface {
public:
    virtual Glib::ustring getFormattedPCIId() = 0;

    virtual uint16_t getVendorPCIId() = 0;

    virtual uint16_t getDevicePCIId() = 0;

    virtual const char *getDeviceRenderNodeName() = 0;
};


#endif //ADRICONF_DRMDEVICEINTERFACE_H
