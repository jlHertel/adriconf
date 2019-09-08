#ifndef ADRICONF_GPUINFO_H
#define ADRICONF_GPUINFO_H

#include <glibmm/ustring.h>
#include <memory>
#include <map>

#include "Section.h"
#include "Application.h"

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

    std::map<Glib::ustring,Glib::ustring> getOptionsMap() const;

    /* Sort the options inside each section to be more user-friendly */
    void sortSectionOptions();

    /* Generate a new application based on this driver-supported options */
    Application_ptr generateApplication() const;
};

typedef std::shared_ptr<GPUInfo> GPUInfo_ptr;

#endif
