#ifndef ADRICONF_DRMDEVICE_H
#define ADRICONF_DRMDEVICE_H

#include "DRMDeviceInterface.h"
#include <xf86drm.h>

class DRMDevice : public DRMDeviceInterface {
private:
    drmDevicePtr rawDevice;

public:
    explicit DRMDevice(drmDevicePtr rawDevice);

    virtual ~DRMDevice();

    Glib::ustring getFormattedPCIId() override;

    uint16_t getVendorPCIId() override;

    uint16_t getDevicePCIId() override;

    const char *getDeviceRenderNodeName() override;
};


#endif //ADRICONF_DRMDEVICE_H
