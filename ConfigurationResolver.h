#ifndef DRICONF3_CONFIGURATIONRESOLVER_H
#define DRICONF3_CONFIGURATIONRESOLVER_H

#include <list>
#include <glibmm/ustring.h>
#include "Device.h"
#include <algorithm>
#include "DriverConfiguration.h"

namespace DRI::ConfigurationResolver {
    /**
     * Takes all the options set and filter out the options already defined system-wide
     * Also filter out any empty application (applications which options are equal to system-wide or driver-default)
     * Filters also the options that have the value equal to driver-default
     * @param systemWideOptions
     * @param driverAvailableOptions
     * @param userDefinedOptions
     * @return A lista of new devices ready to be written to disk
     */
    std::list<std::shared_ptr<DRI::Device>> resolveOptionsForSave(
            const std::shared_ptr<DRI::Device> &,
            const std::list<DRI::DriverConfiguration> &,
            const std::list<std::shared_ptr<DRI::Device>> &
    );

    /**
     * Removes any option that is not supported by this driver
     * This function will directly change the userDefinedOptions list passed as argument
     * @param driverAvailableOptions
     * @param userDefinedOptions
     */
    void filterDriverUnsupportedOptions(
            const std::list<DRI::DriverConfiguration> &,
            std::list<std::shared_ptr<DRI::Device>> &
    );

    /**
     * Merge all the options defined in system-wide config together with the user-defined ones
     * This function will directly change the userDefinedOptions list passed as argument
     * @param systemWideOptions
     * @param driverAvailableOptions
     * @param userDefinedOptions
     */
    void mergeOptionsForDisplay(
            const std::shared_ptr<DRI::Device> &,
            const std::list<DRI::DriverConfiguration> &,
            std::list<std::shared_ptr<DRI::Device>> &
    );
};


#endif
