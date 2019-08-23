#ifndef ADRICONF_CONFIGURATIONRESOLVERINTERFACE_H
#define ADRICONF_CONFIGURATIONRESOLVERINTERFACE_H

#include <list>
#include <map>
#include <glibmm/ustring.h>

#include "../ValueObject/Device.h"
#include "../ValueObject/DriverConfiguration.h"

class ConfigurationResolverInterface {
public:
    virtual ~ConfigurationResolverInterface() = default;

    virtual std::list<Device_ptr> resolveOptionsForSave(
            const std::list<Device_ptr> &,
            const std::list<DriverConfiguration> &,
            const std::list<Device_ptr> &,
            std::map<Glib::ustring, GPUInfo_ptr> &
    ) = 0;

    virtual void filterDriverUnsupportedOptions(
            const std::list<DriverConfiguration> &,
            std::list<Device_ptr> &,
            std::map<Glib::ustring, GPUInfo_ptr> &
    ) = 0;

    virtual void mergeOptionsForDisplay(
            const std::list<Device_ptr> &,
            const std::list<DriverConfiguration> &,
            std::list<Device_ptr> &,
            std::map<Glib::ustring, GPUInfo_ptr> &
    ) = 0;

    virtual void updatePrimeApplications(std::list<Device_ptr> &, const std::map<Glib::ustring, GPUInfo_ptr> &) = 0;

    virtual void addMissingDriverOptions(Application_ptr app, std::map<Glib::ustring, Glib::ustring> driverOptions) = 0;

    virtual void addMissingApplications(const Device_ptr &sourceDevice, Device_ptr &targetDevice) = 0;

    virtual void removeInvalidDrivers(const std::list<DriverConfiguration> &availableDrivers,
                                      std::list<Device_ptr> &userDefinedDevices) = 0;

    virtual void mergeConfigurationOnTopOf(std::list<Device_ptr> &source, const std::list<Device_ptr> &addOnTop) = 0;
};


#endif //ADRICONF_CONFIGURATIONRESOLVERINTERFACE_H