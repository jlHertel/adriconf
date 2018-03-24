#include "ConfigurationResolver.h"
#include "Parser.h"
#include <glibmm/i18n.h>

std::list<Device_ptr> ConfigurationResolver::resolveOptionsForSave(
        const Device_ptr &systemWideDevice,
        const std::list<DriverConfiguration> &driverAvailableOptions,
        const std::list<Device_ptr> &userDefinedDevices,
        std::map<Glib::ustring, GPUInfo_ptr> &availableGPUs
) {
    /* Create the final driverList */
    std::list<Device_ptr> mergedDevices;

    /* Precedence: userDefined > System Wide > Driver Default */
    for (const auto &userDefinedDevice : userDefinedDevices) {
        Device_ptr mergedDevice = std::make_shared<Device>();

        mergedDevice->setDriver(userDefinedDevice->getDriver());
        mergedDevice->setScreen(userDefinedDevice->getScreen());

        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDefinedDevice](const DriverConfiguration &d) {
                                             return d.getScreen() == userDefinedDevice->getScreen()
                                                    && d.getDriverName() == userDefinedDevice->getDriver();
                                         });

        std::map<Glib::ustring, Glib::ustring> realDriverOptions;
        std::map<Glib::ustring, Glib::ustring> driverOptions;

        if (driverConfig != driverAvailableOptions.end()) {
            realDriverOptions = driverConfig->getOptionsMap();
        }

        for (const auto &userDefinedApplication : userDefinedDevice->getApplications()) {
            bool addApplication = false;

            Application_ptr mergedApp = std::make_shared<Application>();
            mergedApp->setExecutable(userDefinedApplication->getExecutable());
            mergedApp->setName(userDefinedApplication->getName());

            driverOptions = realDriverOptions;
            /* PRIME: Check if we should use the app-specific driver options */
            if (userDefinedApplication->getIsUsingPrime()) {
                /* Check for empty because we set this value when no GPU is selected under prime */
                if (!userDefinedApplication->getDevicePCIId().empty()) {
                    driverOptions = availableGPUs[userDefinedApplication->getDevicePCIId()]->getOptionsMap();

                    addApplication = true;
                    ApplicationOption_ptr newMergedOption = std::make_shared<ApplicationOption>();
                    newMergedOption->setName("device_id");
                    newMergedOption->setValue(userDefinedApplication->getDevicePCIId());
                    mergedApp->addOption(newMergedOption);
                }
            }

            Application_ptr systemWideApp = systemWideDevice->findApplication(userDefinedApplication->getExecutable());

            /* If this application already exists systemWide, we need to do a merge on it */
            if (systemWideApp != nullptr) {
                /* List of name-value options */
                std::map<Glib::ustring, Glib::ustring> systemWideAppOptions = systemWideApp->getOptionsAsMap();

                for (auto const &userDefinedAppOption : userDefinedApplication->getOptions()) {
                    if (systemWideAppOptions.count(userDefinedAppOption->getName()) > 0) {
                        /* If the option value is the same as the one used just ignore */
                        if (systemWideAppOptions[userDefinedAppOption->getName()] != userDefinedAppOption->getValue()) {
                            addApplication = true;
                            ApplicationOption_ptr newMergedOption = std::make_shared<ApplicationOption>();
                            newMergedOption->setName(userDefinedAppOption->getName());
                            newMergedOption->setValue(userDefinedAppOption->getValue());
                            mergedApp->addOption(newMergedOption);
                        }
                    } else {
                        /*
                         * DriverOption doesn't exist in system-wide
                         * We must check what is the default value from driver
                         */
                        if (driverOptions.count(userDefinedAppOption->getName()) > 0) {
                            if (driverOptions[userDefinedAppOption->getName()] != userDefinedAppOption->getValue()) {
                                addApplication = true;

                                ApplicationOption_ptr newMergedOption = std::make_shared<ApplicationOption>();
                                newMergedOption->setName(userDefinedAppOption->getName());
                                newMergedOption->setValue(userDefinedAppOption->getValue());
                                mergedApp->addOption(newMergedOption);
                            }
                        }
                    }
                }

                if (addApplication) {
                    mergedDevice->addApplication(mergedApp);
                }
            } else {
                /**
                 * Application doesn't exist in system-wide configuration
                 * but we must check each option to see if its value is the same as the driver default
                 */

                for (auto &userDefinedAppOption : userDefinedApplication->getOptions()) {
                    if (driverOptions.count(userDefinedAppOption->getName()) > 0) {
                        if (driverOptions[userDefinedAppOption->getName()] != userDefinedAppOption->getValue()) {
                            ApplicationOption_ptr newMergedOption = std::make_shared<ApplicationOption>();
                            newMergedOption->setName(userDefinedAppOption->getName());
                            newMergedOption->setValue(userDefinedAppOption->getValue());
                            mergedApp->addOption(newMergedOption);
                        }
                    }
                }

                mergedDevice->addApplication(mergedApp);
            }
        }

        mergedDevices.emplace_back(mergedDevice);
    }

    return mergedDevices;
}

void ConfigurationResolver::filterDriverUnsupportedOptions(
        const std::list<DriverConfiguration> &driverAvailableOptions,
        std::list<Device_ptr> &userDefinedDevices,
        std::map<Glib::ustring, GPUInfo_ptr> &availableGPUs
) {
    // Remove user-defined configurations that don't exists at driver level
    ConfigurationResolver::removeInvalidDrivers(driverAvailableOptions, userDefinedDevices);

    for (auto &userDefinedDevice : userDefinedDevices) {
        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDefinedDevice](const DriverConfiguration &d) {
                                             return (
                                                     d.getScreen() == userDefinedDevice->getScreen()
                                                     &&
                                                     d.getDriverName() == userDefinedDevice->getDriver()
                                             );
                                         });

        std::map<Glib::ustring, Glib::ustring> driverOptions, driverRealOptions;
        Glib::ustring correctDriverName;

        if (driverConfig != driverAvailableOptions.end()) {
            driverOptions = driverConfig->getOptionsMap();
        }

        auto userDefinedApplications = userDefinedDevice->getApplications();
        for (auto &userDefinedApp : userDefinedApplications) {
            auto options = userDefinedApp->getOptions();

            correctDriverName = driverConfig->getDriverName();

            auto deviceOption = std::find_if(options.begin(), options.end(),
                                             [](const ApplicationOption_ptr &option_ptr) {
                                                 return option_ptr->getName() == "device_id";
                                             });

            if (deviceOption == options.end()) {
                driverRealOptions = driverOptions;
            } else {
                Glib::ustring devicePCIId = (*deviceOption)->getValue();
                /* Check if this GPU exists at all */
                if (availableGPUs.count(devicePCIId) == 0) {
                    driverRealOptions = driverOptions;
                } else {
                    GPUInfo_ptr gpuSelected = availableGPUs[devicePCIId];

                    driverRealOptions = gpuSelected->getOptionsMap();
                    correctDriverName = gpuSelected->getDriverName();
                }
            }

            auto itr = options.begin();
            while (itr != options.end()) {
                // Ignore PRIME device option
                if ((*itr)->getName() == "device_id") {
                    ++itr;
                    continue;
                }

                if (driverRealOptions.count((*itr)->getName()) == 0) {
                    std::cerr << Glib::ustring::compose(
                            _("Driver '%1' doesn't support option '%2' on application '%3'. Option removed."),
                            correctDriverName,
                            (*itr)->getName(),
                            userDefinedApp->getName()
                    ) << std::endl;
                    itr = options.erase(itr);
                } else {
                    ++itr;
                }
            }

            userDefinedApp->setOptions(options);
        }
    }
}

void ConfigurationResolver::mergeOptionsForDisplay(
        const Device_ptr &systemWideDevice,
        const std::list<DriverConfiguration> &driverAvailableOptions,
        std::list<Device_ptr> &userDefinedOptions,
        std::map<Glib::ustring, GPUInfo_ptr> &availableGPUs
) {
    for (const auto &driverConf : driverAvailableOptions) {
        /* Check if user-config has any config for this screen/driver */
        auto userSearchDefinedDevice = std::find_if(userDefinedOptions.begin(), userDefinedOptions.end(),
                                                    [&driverConf](const Device_ptr &d) {
                                                        return d->getDriver() == driverConf.getDriverName()
                                                               && d->getScreen() == driverConf.getScreen();
                                                    });
        Device_ptr userDefinedDevice = nullptr;

        bool addDeviceToList = false;

        if (userSearchDefinedDevice == userDefinedOptions.end()) {
            userDefinedDevice = std::make_shared<Device>();
            userDefinedDevice->setDriver(driverConf.getDriverName());
            userDefinedDevice->setScreen(driverConf.getScreen());
            addDeviceToList = true;
        } else {
            userDefinedDevice = *userSearchDefinedDevice;
        }

        std::map<Glib::ustring, Glib::ustring> realDriverOptions = driverConf.getOptionsMap();
        std::map<Glib::ustring, Glib::ustring> driverOptions;

        std::list<Application_ptr> newDeviceApps = userDefinedDevice->getApplications();

        /* Check if the user-defined apps are missing any of the driver option */
        for (auto &userDefinedApp : newDeviceApps) {
            driverOptions = realDriverOptions;
            if (userDefinedApp->getIsUsingPrime()) {
                /* Check for empty because we set this value when no GPU is selected under prime */
                if (!userDefinedApp->getDevicePCIId().empty()) {
                    driverOptions = availableGPUs[userDefinedApp->getDevicePCIId()]->getOptionsMap();
                }
            }

            for (const auto &driverDefinedOption : driverOptions) {
                auto optionExists = std::find_if(userDefinedApp->getOptions().begin(),
                                                 userDefinedApp->getOptions().end(),
                                                 [&driverDefinedOption](const ApplicationOption_ptr &o) {
                                                     return o->getName() == driverDefinedOption.first;
                                                 });
                /* Option doesn't exists, lets add it */
                if (optionExists == userDefinedApp->getOptions().end()) {
                    ApplicationOption_ptr newDriverOpt = std::make_shared<ApplicationOption>();
                    newDriverOpt->setName(driverDefinedOption.first);
                    newDriverOpt->setValue(driverDefinedOption.second);

                    userDefinedApp->addOption(newDriverOpt);
                }
            }
        }

        driverOptions = realDriverOptions;

        /* Check if we can add any of the system-wide apps for this config */
        for (const auto &systemWideApp : systemWideDevice->getApplications()) {
            auto appExists = std::find_if(newDeviceApps.begin(), newDeviceApps.end(),
                                          [&systemWideApp](const Application_ptr &app) {
                                              return app->getExecutable() == systemWideApp->getExecutable();
                                          });

            if (appExists == newDeviceApps.end()) {
                Application_ptr systemDefinedApp = std::make_shared<Application>();
                systemDefinedApp->setName(systemWideApp->getName());
                systemDefinedApp->setExecutable(systemWideApp->getExecutable());

                for (const auto &driverOptionObj : driverOptions) {
                    auto optionExists = std::find_if(systemWideApp->getOptions().begin(),
                                                     systemWideApp->getOptions().end(),
                                                     [&driverOptionObj](const ApplicationOption_ptr &a) {
                                                         return driverOptionObj.first == a->getName();
                                                     });

                    if (optionExists == systemWideApp->getOptions().end()) {
                        ApplicationOption_ptr newDriverOpt = std::make_shared<ApplicationOption>();
                        newDriverOpt->setName(driverOptionObj.first);
                        newDriverOpt->setValue(driverOptionObj.second);

                        systemDefinedApp->addOption(newDriverOpt);
                    } else {
                        /* Option already exists, lets add it */
                        ApplicationOption_ptr newDriverOpt = std::make_shared<ApplicationOption>();
                        newDriverOpt->setName((*optionExists)->getName());
                        newDriverOpt->setValue((*optionExists)->getValue());

                        systemDefinedApp->addOption(newDriverOpt);
                    }
                }

                userDefinedDevice->addApplication(systemDefinedApp);
            }
        }

        /* Check if we have a default config */
        auto defaultApp = std::find_if(newDeviceApps.begin(), newDeviceApps.end(),
                                       [](const Application_ptr &app) {
                                           return app->getExecutable().empty();
                                       });

        if (defaultApp == newDeviceApps.end()) {
            Application_ptr defaultApplication = std::make_shared<Application>();
            defaultApplication->setName("Default");
            auto defaultAppOptions = defaultApplication->getOptions();
            for (auto &driverOptionObj : driverOptions) {
                ApplicationOption_ptr newDriverOpt = std::make_shared<ApplicationOption>();
                newDriverOpt->setName(driverOptionObj.first);
                newDriverOpt->setValue(driverOptionObj.second);

                defaultApplication->addOption(newDriverOpt);
            }

            userDefinedDevice->addApplication(defaultApplication);
        }

        if (addDeviceToList) {
            userDefinedOptions.emplace_back(userDefinedDevice);
        }
    }
}

void ConfigurationResolver::updatePrimeApplications(std::list<Device_ptr> &userDefinedDevices,
                                                    const std::map<Glib::ustring, GPUInfo_ptr> &availableGPUs) {
    for (auto &device : userDefinedDevices) {
        for (auto &app : device->getApplications()) {
            app->setIsUsingPrime(false);
            auto primeOption = std::find_if(app->getOptions().begin(), app->getOptions().end(),
                                            [](const ApplicationOption_ptr &o) {
                                                return o->getName() == "device_id";
                                            });

            if (primeOption != app->getOptions().end()) {
                auto foundGpu = availableGPUs.find((*primeOption)->getValue());
                if (foundGpu != availableGPUs.end()) {
                    app->setIsUsingPrime(true);
                    app->setPrimeDriverName(foundGpu->second->getDriverName());
                    app->setDevicePCIId(foundGpu->second->getPciId());
                }
            }
        }
    }
}

void ConfigurationResolver::addMissingDriverOptions(Application_ptr app,
                                                    std::map<Glib::ustring, Glib::ustring> driverOptions) {

    std::map<Glib::ustring, Glib::ustring> appOptions = app->getOptionsAsMap();
    for (auto driverOpt : driverOptions) {
        if (appOptions.count(driverOpt.first) == 0) {
            ApplicationOption_ptr newOption = std::make_shared<ApplicationOption>();
            newOption->setName(driverOpt.first);
            newOption->setValue(driverOpt.second);

            app->addOption(newOption);
        }
    }
}

void ConfigurationResolver::removeInvalidDrivers(
        const std::list<DriverConfiguration> &availableDrivers,
        std::list<Device_ptr> &userDefinedDevices
) {
    auto deviceIterator = userDefinedDevices.begin();
    while (deviceIterator != userDefinedDevices.end()) {
        Glib::ustring currentUserDefinedDriver((*deviceIterator)->getDriver());
        int currentUserDefinedScreen = (*deviceIterator)->getScreen();
        auto driverSupports = std::find_if(availableDrivers.begin(), availableDrivers.end(),
                                           [&currentUserDefinedDriver, &currentUserDefinedScreen](
                                                   const DriverConfiguration &d) {
                                               return (
                                                       d.getDriverName() == currentUserDefinedDriver
                                                       &&
                                                       d.getScreen() == currentUserDefinedScreen
                                               );
                                           });

        if (driverSupports == availableDrivers.end()) {
            std::cerr << Glib::ustring::compose(
                    _("User-defined driver '%1' on screen '%2' doesn't have a driver loaded on system. Configuration removed."),
                    currentUserDefinedDriver,
                    currentUserDefinedScreen
            ) << std::endl;

            deviceIterator = userDefinedDevices.erase(deviceIterator);
        } else {
            ++deviceIterator;
        }
    }
}
