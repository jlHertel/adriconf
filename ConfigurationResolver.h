#ifndef DRICONF3_CONFIGURATIONRESOLVER_H
#define DRICONF3_CONFIGURATIONRESOLVER_H

#include <list>
#include <glibmm/ustring.h>
#include "Device.h"
#include <algorithm>
#include "DriverConfiguration.h"

namespace DRI::ConfigurationResolver {
    std::list<DRI::Device> resolveOptionsForSave(
            const DRI::Device &systemWideOptions,
            const std::list<DRI::DriverConfiguration> &driverAvailableOptions,
            std::list<DRI::Device> userDefinedOptions
    );

    std::list<DRI::Device> filterDriverUnsupportedOptions(
            std::list<DRI::DriverConfiguration> driverAvailableOptions,
            std::list<DRI::Device> userDefinedOptions
    );
};


#endif
