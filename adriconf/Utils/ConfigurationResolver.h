#ifndef DRICONF3_CONFIGURATIONRESOLVER_H
#define DRICONF3_CONFIGURATIONRESOLVER_H

#include <list>
#include <map>
#include <glibmm/ustring.h>
#include <algorithm>

#include "../ValueObject/Device.h"
#include "../ValueObject/DriverConfiguration.h"
#include "../ValueObject/GPUInfo.h"
#include "../Logging/LoggerInterface.h"

namespace ConfigurationResolver {
    /**
     * Takes all the options set and filter out the options already defined system-wide
     * Filters also the options that have the value equal to driver-default
     * @param systemWideOptions
     * @param driverAvailableOptions
     * @param userDefinedOptions
     * @param availableGPUs
     * @return A lista of new devices ready to be written to disk
     */
    std::list<Device_ptr> resolveOptionsForSave(
            const std::list<Device_ptr> &,
            const std::list<DriverConfiguration> &,
            const std::list<Device_ptr> &,
            std::map<Glib::ustring, GPUInfo_ptr> &
    );

    /**
     * Removes any option that is not supported by this driver
     * This function will directly change the userDefinedOptions list passed as argument
     * @param driverAvailableOptions
     * @param userDefinedOptions
     * @param availableGPUs
     */
    void filterDriverUnsupportedOptions(
            const std::list<DriverConfiguration> &,
            std::list<Device_ptr> &,
            std::map<Glib::ustring, GPUInfo_ptr> &,
            LoggerInterface *logger
    );

    /**
     * Merge all the options defined in system-wide config together with the user-defined ones
     * This function will directly change the userDefinedOptions list passed as argument
     * @param systemWideOptions
     * @param driverAvailableOptions
     * @param userDefinedOptions
     * @param availableGPUs
     */
    void mergeOptionsForDisplay(
            const std::list<Device_ptr> &,
            const std::list<DriverConfiguration> &,
            std::list<Device_ptr> &,
            std::map<Glib::ustring, GPUInfo_ptr> &
    );

    /**
     * For each application check if it has a device_id defined and update its driver name accordingly
     */
    void updatePrimeApplications(std::list<Device_ptr> &, const std::map<Glib::ustring, GPUInfo_ptr> &);

    void addMissingDriverOptions(Application_ptr app, std::map<Glib::ustring, Glib::ustring> driverOptions);

    void addMissingApplications(const Device_ptr &sourceDevice, Device_ptr &targetDevice);

    void removeInvalidDrivers(const std::list<DriverConfiguration> &availableDrivers,
                              std::list<Device_ptr> &userDefinedDevices,
                              LoggerInterface *logger);

    void mergeConfigurationOnTopOf(std::list<Device_ptr> &source, const std::list<Device_ptr> &addOnTop);
}


#endif
