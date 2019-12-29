#ifndef ADRICONF_GBMDEVICE_H
#define ADRICONF_GBMDEVICE_H

#include <gbm.h>

class GBMDevice {
private:
    int rawFileDescriptor;
    struct gbm_device *rawGBMDevice;

public:
    GBMDevice();

    virtual ~GBMDevice();

    void setRawFileDescriptor(int fileDescriptor);

    void setRawGBMDevice(struct gbm_device *device);

    struct gbm_device *getRawGBMDevice() const;
};

#endif //ADRICONF_GBMDEVICE_H