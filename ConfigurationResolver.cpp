#include "ConfigurationResolver.h"
#include "Parser.h"
#include <glibmm/i18n.h>

std::list<Device_ptr> ConfigurationResolver::resolveOptionsForSave(
        const Device_ptr &systemWideDevice,
        const std::list<DriverConfiguration> &driverAvailableOptions,
        const std::list<Device_ptr> &userDefinedDevices
) {
    /* Create the final driverList */
    std::list<Device_ptr> mergedDevices;

    /* Precedence: userDefined > System Wide > Driver Default */
    for (const auto &userDefinedDevice : userDefinedDevices) {
        auto mergedDevice = std::make_shared<Device>();

        mergedDevice->setDriver(userDefinedDevice->getDriver());
        mergedDevice->setScreen(userDefinedDevice->getScreen());

        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDefinedDevice](const DriverConfiguration &d) {
                                             return d.getScreen() == userDefinedDevice->getScreen();
                                         });

        std::list<DriverOption> driverOptions;

        if (driverConfig != driverAvailableOptions.end()) {
            driverOptions = Parser::convertSectionsToOptionsObject(driverConfig->getSections());
        }

        for (const auto &userDefinedApplication : userDefinedDevice->getApplications()) {
            auto mergedApp = std::make_shared<Application>();
            mergedApp->setExecutable(userDefinedApplication->getExecutable());
            mergedApp->setName(userDefinedApplication->getName());

            auto systemWideApp = systemWideDevice->findApplication(userDefinedApplication->getExecutable());

            /* If this application already exists systemWide, we need to do a merge on it */
            if (systemWideApp != nullptr) {
                bool addApplication = false;

                /* List of name-value options */
                auto systemWideAppOptions = systemWideApp->getOptions();


                for (auto const &userDefinedAppOption : userDefinedApplication->getOptions()) {
                    auto systemWideAppOption = std::find_if(systemWideAppOptions.begin(), systemWideAppOptions.end(),
                                                            [&userDefinedAppOption](ApplicationOption_ptr a) {
                                                                return a->getName() == userDefinedAppOption->getName();
                                                            });

                    if (systemWideAppOption != systemWideAppOptions.end()) {
                        /* If the option set is the same as the one used just ignore this options*/
                        if ((*systemWideAppOption)->getValue() != userDefinedAppOption->getValue()) {
                            addApplication = true;
                            auto newMergedOption = std::make_shared<ApplicationOption>();
                            newMergedOption->setName(userDefinedAppOption->getName());
                            newMergedOption->setValue(userDefinedAppOption->getValue());
                            mergedApp->addOption(newMergedOption);
                        }
                    } else {
                        /*
                         * DriverOption doesn't exist in system-wide
                         * We must check what is the default value from driver
                         */
                        auto driverOption = std::find_if(driverOptions.begin(), driverOptions.end(),
                                                         [&userDefinedAppOption](const DriverOption &d) {
                                                             return d.getName() == userDefinedAppOption->getName();
                                                         });

                        if (driverOption->getDefaultValue() != userDefinedAppOption->getValue()) {
                            addApplication = true;

                            auto newMergedOption = std::make_shared<ApplicationOption>();
                            newMergedOption->setName(userDefinedAppOption->getName());
                            newMergedOption->setValue(userDefinedAppOption->getValue());
                            mergedApp->addOption(newMergedOption);
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
                    auto driverOption = std::find_if(driverOptions.begin(), driverOptions.end(),
                                                     [&userDefinedAppOption](DriverOption &d) {
                                                         return d.getName() == userDefinedAppOption->getName();
                                                     });

                    if (driverOption->getDefaultValue() != userDefinedAppOption->getValue()) {
                        auto newMergedOption = std::make_shared<ApplicationOption>();
                        newMergedOption->setName(userDefinedAppOption->getName());
                        newMergedOption->setValue(userDefinedAppOption->getValue());
                        mergedApp->addOption(newMergedOption);
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
        std::list<Device_ptr> &userDefinedDevices
) {
    // Remove user-defined configurations that don't exists at driver level
    auto deviceIterator = userDefinedDevices.begin();
    while (deviceIterator != userDefinedDevices.end()) {
        Glib::ustring currentUserDefinedDriver((*deviceIterator)->getDriver());
        int currentUserDefinedScreen = (*deviceIterator)->getScreen();
        auto driverSupports = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                           [&currentUserDefinedDriver, &currentUserDefinedScreen](
                                                   const DriverConfiguration &d) {
                                               return (
                                                       d.getDriver() == currentUserDefinedDriver
                                                       &&
                                                       d.getScreen() == currentUserDefinedScreen
                                               );
                                           });

        if (driverSupports == driverAvailableOptions.end()) {
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

    for (auto &userDefinedDevice : userDefinedDevices) {
        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDefinedDevice](const DriverConfiguration &d) {
                                             return (
                                                     d.getScreen() == userDefinedDevice->getScreen()
                                                     &&
                                                     d.getDriver() == userDefinedDevice->getDriver()
                                             );
                                         });

        std::list<Glib::ustring> driverOptions;

        if (driverConfig != driverAvailableOptions.end()) {
            driverOptions = Parser::convertSectionsToOptions(driverConfig->getSections());
        }

        auto userDefinedApplications = userDefinedDevice->getApplications();
        for (auto &userDefinedApp : userDefinedApplications) {
            auto options = userDefinedApp->getOptions();

            auto itr = options.begin();
            while (itr != options.end()) {
                auto driverSupports = std::find(driverOptions.begin(), driverOptions.end(), (*itr)->getName());

                if (driverSupports == driverOptions.end()) {
                    std::cerr << Glib::ustring::compose(
                            _("Driver '%1' doesn't support option '%2' on application '%3'. Option removed."),
                            driverConfig->getDriver(),
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
        std::list<Device_ptr> &userDefinedOptions
) {
    for (const auto &driverConf : driverAvailableOptions) {
        /* Check if user-config has any config for this screen/driver */
        auto userSearchDefinedDevice = std::find_if(userDefinedOptions.begin(), userDefinedOptions.end(),
                                                    [&driverConf](Device_ptr d) {
                                                        return d->getDriver() == driverConf.getDriver()
                                                               && d->getScreen() == driverConf.getScreen();
                                                    });
        Device_ptr userDefinedDevice = nullptr;

        bool addDeviceToList = false;

        if (userSearchDefinedDevice == userDefinedOptions.end()) {
            userDefinedDevice = std::make_shared<Device>();
            userDefinedDevice->setDriver(driverConf.getDriver());
            userDefinedDevice->setScreen(driverConf.getScreen());
            addDeviceToList = true;
        } else {
            userDefinedDevice = *userSearchDefinedDevice;
        }

        auto driverOptions = Parser::convertSectionsToOptionsObject(driverConf.getSections());

        auto newDeviceApps = userDefinedDevice->getApplications();

        /* Check if the user-defined apps are missing any of the driver option */
        for (auto &userDefinedApp : newDeviceApps) {
            for (const auto &driverDefinedOption : driverOptions) {
                auto optionExists = std::find_if(userDefinedApp->getOptions().begin(),
                                                 userDefinedApp->getOptions().end(),
                                                 [&driverDefinedOption](ApplicationOption_ptr o) {
                                                     return o->getName() == driverDefinedOption.getName();

                                                 });
                /* Option doesn't exists, lets add it */
                if (optionExists == userDefinedApp->getOptions().end()) {
                    auto newDriverOpt = std::make_shared<ApplicationOption>();
                    newDriverOpt->setName(driverDefinedOption.getName());
                    newDriverOpt->setValue(driverDefinedOption.getDefaultValue());

                    userDefinedApp->addOption(newDriverOpt);
                }
            }
        }


        /* Check if we can add any of the system-wide apps for this config */
        for (const auto &systemWideApp : systemWideDevice->getApplications()) {
            auto appExists = std::find_if(newDeviceApps.begin(), newDeviceApps.end(),
                                          [&systemWideApp](Application_ptr app) {
                                              return app->getExecutable() == systemWideApp->getExecutable();
                                          });

            if (appExists == newDeviceApps.end()) {
                auto systemDefinedApp = std::make_shared<Application>();
                systemDefinedApp->setName(systemWideApp->getName());
                systemDefinedApp->setExecutable(systemWideApp->getExecutable());

                for (const auto &driverOptionObj : driverOptions) {
                    auto optionExists = std::find_if(systemWideApp->getOptions().begin(),
                                                     systemWideApp->getOptions().end(),
                                                     [&driverOptionObj](ApplicationOption_ptr a) {
                                                         return driverOptionObj.getName() == a->getName();
                                                     });

                    if (optionExists == systemWideApp->getOptions().end()) {
                        auto newDriverOpt = std::make_shared<ApplicationOption>();
                        newDriverOpt->setName(driverOptionObj.getName());
                        newDriverOpt->setValue(driverOptionObj.getDefaultValue());

                        systemDefinedApp->addOption(newDriverOpt);
                    } else {
                        /* Option already exists, lets add it */
                        auto newDriverOpt = std::make_shared<ApplicationOption>();
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
                                       [](Application_ptr app) {
                                           return app->getExecutable().empty();
                                       });

        if (defaultApp == newDeviceApps.end()) {
            auto defaultApplication = std::make_shared<Application>();
            defaultApplication->setName("Default");
            auto defaultAppOptions = defaultApplication->getOptions();
            for (auto &driverOptionObj : driverOptions) {
                auto newDriverOpt = std::make_shared<ApplicationOption>();
                newDriverOpt->setName(driverOptionObj.getName());
                newDriverOpt->setValue(driverOptionObj.getDefaultValue());

                defaultApplication->addOption(newDriverOpt);
            }

            userDefinedDevice->addApplication(defaultApplication);
        }

        if (addDeviceToList) {
            userDefinedOptions.emplace_back(userDefinedDevice);
        }
    }
}