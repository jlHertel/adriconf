//
// Created by jean on 29/12/2019.
//

#include "GBMDevice.h"

#include <unistd.h>

GBMDevice::~GBMDevice() {
    if (this->rawGBMDevice != nullptr) {
        gbm_device_destroy(this->rawGBMDevice);
    }

    if (this->rawFileDescriptor != 0) {
        close(this->rawFileDescriptor);
    }
}

GBMDevice::GBMDevice() : rawFileDescriptor(0), rawGBMDevice(nullptr) {}

void GBMDevice::setRawFileDescriptor(int fileDescriptor) {
    this->rawFileDescriptor = fileDescriptor;
}

void GBMDevice::setRawGBMDevice(struct gbm_device *device) {
    this->rawGBMDevice = device;
}

struct gbm_device *GBMDevice::getRawGBMDevice() const {
    return this->rawGBMDevice;
}

