#include "GBMDeviceFactory.h"
#include <fcntl.h>
#include <stdexcept>
#include <glibmm/i18n.h>
#include <glibmm/ustring.h>

GBMDevice GBMDeviceFactory::generateDeviceFromPath(const char *path) {
    int fileDescriptor = open(path, O_RDWR | FD_CLOEXEC);
    if (fileDescriptor <= 0) {
        throw std::runtime_error(Glib::ustring::compose(_("Failed to open device %1"), path));
    }

    struct gbm_device *gbm = gbm_create_device(fileDescriptor);
    if (gbm == nullptr) {
        throw std::runtime_error(Glib::ustring::compose(_("Failed to create GBM device for %1"), path));
    }

    GBMDevice device;
    device.setRawFileDescriptor(fileDescriptor);
    device.setRawGBMDevice(gbm);

    return device;
}
