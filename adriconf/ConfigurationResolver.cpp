#include "ConfigurationResolver.h"
#include "Parser.h"
#include "LoggerInterface.h"
#include <glibmm/i18n.h>

std::list<Device_ptr> ConfigurationResolver::resolveOptionsForSave(
        const std::list<Device_ptr> &systemWideDevices,
        const std::list<DriverConfiguration> &driverAvailableOptions,
        const std::list<Device_ptr> &userDefinedDevices,
        std::map<Glib::ustring, GPUInfo_ptr> &availableGPUs
) {
    /* Create the final driverList */
    std::list<Device_ptr> mergedDevices;

    Device_ptr defaultSystemWideDevice;

    for (const auto &systemDeviceSearch : systemWideDevices) {
        if (systemDeviceSearch->getDriver().empty()) {
            defaultSystemWideDevice = systemDeviceSearch;
            break;
        }
    }

    /* Precedence: userDefined > System Wide > Driver Default */
    for (const auto &userDefinedDevice : userDefinedDevices) {
        /* Search if there is any device like this system-wide */
        Device_ptr systemWideDevice = nullptr;
        for (const auto &systemDeviceSearch : systemWideDevices) {
            if (systemDeviceSearch->getDriver() == userDefinedDevice->getDriver()) {
                systemWideDevice = systemDeviceSearch;
                break;
            }
        }

        if (systemWideDevice == nullptr) {
            systemWideDevice = defaultSystemWideDevice;
        }


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
        std::map<Glib::ustring, GPUInfo_ptr> &availableGPUs,
        LoggerInterface *logger
) {
    // Remove user-defined configurations that don't exists at driver level
    ConfigurationResolver::removeInvalidDrivers(driverAvailableOptions, userDefinedDevices, logger);

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
                    logger->warning(Glib::ustring::compose(
                            _("Driver '%1' doesn't support option '%2' on application '%3'. Option removed."),
                            correctDriverName,
                            (*itr)->getName(),
                            userDefinedApp->getName()
                    ));
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
        const std::list<Device_ptr> &systemWideDevices,
        const std::list<DriverConfiguration> &driverAvailableOptions,
        std::list<Device_ptr> &userDefinedOptions,
        std::map<Glib::ustring, GPUInfo_ptr> &availableGPUs
) {
    Device_ptr defaultSystemWideDevice;

    for (const auto &systemDeviceSearch : systemWideDevices) {
        if (systemDeviceSearch->getDriver().empty()) {
            defaultSystemWideDevice = systemDeviceSearch;
            break;
        }
    }

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

        /* Search if there is any device like this system-wide */
        Device_ptr systemWideDevice = nullptr;
        for (const auto &systemDeviceSearch : systemWideDevices) {
            if (systemDeviceSearch->getDriver() == userDefinedDevice->getDriver()) {
                systemWideDevice = systemDeviceSearch;
                break;
            }
        }

        if (systemWideDevice == nullptr) {
            systemWideDevice = defaultSystemWideDevice;
        }


        std::map<Glib::ustring, Glib::ustring> realDriverOptions = driverConf.getOptionsMap();
        std::map<Glib::ustring, Glib::ustring> driverOptions;

        /* Check if we can add any of the system-wide apps for this config */
        ConfigurationResolver::addMissingApplications(systemWideDevice, userDefinedDevice);

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

            ConfigurationResolver::addMissingDriverOptions(userDefinedApp, driverOptions);
        }

        /* Check if we have a default config */
        auto defaultApp = std::find_if(newDeviceApps.begin(), newDeviceApps.end(),
                                       [](const Application_ptr &app) {
                                           return app->getExecutable().empty();
                                       });

        if (defaultApp == newDeviceApps.end()) {
            Application_ptr defaultApplication = std::make_shared<Application>();
            defaultApplication->setName("Default");

            ConfigurationResolver::addMissingDriverOptions(defaultApplication, realDriverOptions);

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
        std::list<Device_ptr> &userDefinedDevices,
        LoggerInterface *logger
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
            logger->warning(Glib::ustring::compose(
                    _("User-defined driver '%1' on screen '%2' doesn't have a driver loaded on system. Configuration removed."),
                    currentUserDefinedDriver,
                    currentUserDefinedScreen
            ));

            deviceIterator = userDefinedDevices.erase(deviceIterator);
        } else {
            ++deviceIterator;
        }
    }
}

void ConfigurationResolver::addMissingApplications(const Device_ptr &sourceDevice, Device_ptr &targetDevice) {
    for (const auto &sourceApplication : sourceDevice->getApplications()) {
        auto appExists = std::find_if(targetDevice->getApplications().begin(), targetDevice->getApplications().end(),
                                      [&sourceApplication](const Application_ptr &app) {
                                          return app->getExecutable() == sourceApplication->getExecutable();
                                      });

        if (appExists == targetDevice->getApplications().end()) {
            Application_ptr newApplication = std::make_shared<Application>();
            newApplication->setName(sourceApplication->getName());
            newApplication->setExecutable(sourceApplication->getExecutable());

            for (const auto &sourceOption : sourceApplication->getOptions()) {
                ApplicationOption_ptr newOption = std::make_shared<ApplicationOption>();
                newOption->setName(sourceOption->getName());
                newOption->setValue(sourceOption->getValue());

                newApplication->addOption(newOption);
            }

            targetDevice->addApplication(newApplication);
        }
    }
}

void
ConfigurationResolver::mergeConfigurationOnTopOf(std::list<Device_ptr> &source, const std::list<Device_ptr> &addOnTop) {
    for (const auto &deviceToBeAdded : addOnTop) {
        auto existingDevice = std::find_if(source.begin(), source.end(),
                                           [&deviceToBeAdded](const Device_ptr &searchingDevice) {
                                               return deviceToBeAdded->getDriver() == searchingDevice->getDriver()
                                                      && deviceToBeAdded->getScreen() == searchingDevice->getScreen();
                                           }
        );

        if (existingDevice == source.end()) {
            source.emplace_back(deviceToBeAdded);
            continue;
        }

        for (const Application_ptr &appToBeAdded : deviceToBeAdded->getApplications()) {
            auto existingApp = std::find_if((*existingDevice)->getApplications().begin(),
                                            (*existingDevice)->getApplications().end(),
                                            [&appToBeAdded](const Application_ptr &app) {
                                                return app->getExecutable() == appToBeAdded->getExecutable();
                                            }
            );

            if (existingApp == (*existingDevice)->getApplications().end()) {
                (*existingDevice)->addApplication(appToBeAdded);
                continue;
            }

            for (const ApplicationOption_ptr &optionToBeAdded : appToBeAdded->getOptions()) {
                auto existingOption = std::find_if((*existingApp)->getOptions().begin(),
                                                   (*existingApp)->getOptions().end(),
                                                   [&optionToBeAdded](const ApplicationOption_ptr &option) {
                                                       return optionToBeAdded->getName() == option->getName();
                                                   }
                );

                if (existingOption == (*existingApp)->getOptions().end()) {
                    (*existingApp)->addOption(optionToBeAdded);
                } else {
                    (*existingOption)->setValue(optionToBeAdded->getValue());
                }
            }

        }

    }
}
