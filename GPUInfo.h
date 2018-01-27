#ifndef ADRICONF_GPUINFO_H
#define ADRICONF_GPUINFO_H

#include <glibmm/ustring.h>
#include <memory>

class GPUInfo {
private:
    Glib::ustring pciId;
    Glib::ustring driverName;
    Glib::ustring deviceName;
    Glib::ustring vendorName;

public:
    const Glib::ustring &getPciId() const;

    void setPciId(const Glib::ustring &pciId);

    const Glib::ustring &getDriverName() const;

    void setDriverName(const Glib::ustring &driverName);

    const Glib::ustring &getDeviceName() const;

    void setDeviceName(const Glib::ustring &deviceName);

    const Glib::ustring &getVendorName() const;

    void setVendorName(const Glib::ustring &vendorName);
};

typedef std::shared_ptr<GPUInfo> GPUInfo_ptr;

#endif
