#ifndef ADRICONF_GPUINFO_H
#define ADRICONF_GPUINFO_H

#include <glibmm/ustring.h>
#include <memory>

#include "Section.h"

class GPUInfo {
private:
    Glib::ustring pciId;
    Glib::ustring driverName;
    Glib::ustring deviceName;
    Glib::ustring vendorName;
    uint16_t vendorId;
    uint16_t deviceId;
    std::list<Section> sections;

public:
    const Glib::ustring &getPciId() const;

    void setPciId(const Glib::ustring &pciId);

    const Glib::ustring &getDriverName() const;

    void setDriverName(const Glib::ustring &driverName);

    const Glib::ustring &getDeviceName() const;

    void setDeviceName(const Glib::ustring &deviceName);

    const Glib::ustring &getVendorName() const;

    void setVendorName(const Glib::ustring &vendorName);

    uint16_t getVendorId() const;

    void setVendorId(uint16_t vendorId);

    uint16_t getDeviceId() const;

    void setDeviceId(uint16_t deviceId);

    bool operator==(const GPUInfo &rhs);

    const std::list<Section> &getSections() const;

    void setSections(const std::list<Section> &sections);
};

typedef std::shared_ptr<GPUInfo> GPUInfo_ptr;

#endif
