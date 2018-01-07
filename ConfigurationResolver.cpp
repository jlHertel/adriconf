#include "ConfigurationResolver.h"
#include "Parser.h"
#include <glibmm/i18n.h>

std::list<std::shared_ptr<DRI::Device>> DRI::ConfigurationResolver::resolveOptionsForSave(
        const std::shared_ptr<DRI::Device> &systemWideDevice,
        const std::list<DRI::DriverConfiguration> &driverAvailableOptions,
        const std::list<std::shared_ptr<DRI::Device>> &userDefinedDevices
) {
    /* Create the final driverList */
    std::list<std::shared_ptr<DRI::Device>> mergedDevices;

    /* Precedence: userDefined > System Wide > Driver Default */
    for (const auto &userDefinedDevice : userDefinedDevices) {
        auto mergedDevice = std::make_shared<DRI::Device>();

        mergedDevice->setDriver(userDefinedDevice->getDriver());
        mergedDevice->setScreen(userDefinedDevice->getScreen());

        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDefinedDevice](const DRI::DriverConfiguration &d) {
                                             return d.getScreen() == userDefinedDevice->getScreen();
                                         });

        std::list<DRI::DriverOption> driverOptions;

        if (driverConfig != driverAvailableOptions.end()) {
            driverOptions = DRI::Parser::convertSectionsToOptionsObject(driverConfig->getSections());
        }

        for (const auto &userDefinedApplication : userDefinedDevice->getApplications()) {
            auto mergedApp = std::make_shared<DRI::Application>();
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
                                                            [&userDefinedAppOption](
                                                                    std::shared_ptr<DRI::ApplicationOption> a) {
                                                                return a->getName() == userDefinedAppOption->getName();
                                                            });

                    if (systemWideAppOption != systemWideAppOptions.end()) {
                        /* If the option set is the same as the one used just ignore this options*/
                        if ((*systemWideAppOption)->getValue() != userDefinedAppOption->getValue()) {
                            addApplication = true;
                            auto newMergedOption = std::make_shared<DRI::ApplicationOption>();
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
                                                         [&userDefinedAppOption](const DRI::DriverOption &d) {
                                                             return d.getName() == userDefinedAppOption->getName();
                                                         });

                        if (driverOption->getDefaultValue() != userDefinedAppOption->getValue()) {
                            addApplication = true;

                            auto newMergedOption = std::make_shared<DRI::ApplicationOption>();
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
                bool addApplication = false;

                for (auto &userDefinedAppOption : userDefinedApplication->getOptions()) {
                    auto driverOption = std::find_if(driverOptions.begin(), driverOptions.end(),
                                                     [&userDefinedAppOption](DRI::DriverOption &d) {
                                                         return d.getName() == userDefinedAppOption->getName();
                                                     });

                    if (driverOption->getDefaultValue() != userDefinedAppOption->getValue()) {
                        addApplication = true;

                        auto newMergedOption = std::make_shared<DRI::ApplicationOption>();
                        newMergedOption->setName(userDefinedAppOption->getName());
                        newMergedOption->setValue(userDefinedAppOption->getValue());
                        mergedApp->addOption(newMergedOption);
                    }
                }

                if (addApplication) {
                    mergedDevice->addApplication(mergedApp);
                }
            }
        }

        mergedDevices.emplace_back(mergedDevice);
    }

    return mergedDevices;
}

void DRI::ConfigurationResolver::filterDriverUnsupportedOptions(
        const std::list<DRI::DriverConfiguration> &driverAvailableOptions,
        std::list<std::shared_ptr<DRI::Device>> &userDefinedDevices
) {
    for (auto &userDefinedDevice : userDefinedDevices) {
        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDefinedDevice](const DRI::DriverConfiguration &d) {
                                             return d.getScreen() == userDefinedDevice->getScreen();
                                         });

        std::list<Glib::ustring> driverOptions;

        if (driverConfig != driverAvailableOptions.end()) {
            driverOptions = DRI::Parser::convertSectionsToOptions(driverConfig->getSections());
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

                    auto optionsObject = (*itr);
                } else {
                    ++itr;
                }
            }

            userDefinedApp->setOptions(options);
        }
    }
}

void DRI::ConfigurationResolver::mergeOptionsForDisplay(
        const std::shared_ptr<DRI::Device> &systemWideDevice,
        const std::list<DRI::DriverConfiguration> &driverAvailableOptions,
        std::list<std::shared_ptr<DRI::Device>> &userDefinedOptions
) {
    for (const auto &driverConf : driverAvailableOptions) {
        /* Check if user-config has any config for this screen/driver */
        auto userSearchDefinedDevice = std::find_if(userDefinedOptions.begin(), userDefinedOptions.end(),
                                                    [&driverConf](std::shared_ptr<DRI::Device> d) {
                                                        return d->getDriver() == driverConf.getDriver()
                                                               && d->getScreen() == driverConf.getScreen();
                                                    });
        std::shared_ptr<DRI::Device> userDefinedDevice = nullptr;

        if (userSearchDefinedDevice == userDefinedOptions.end()) {
            userDefinedDevice = std::make_shared<DRI::Device>();
            userDefinedDevice->setDriver(driverConf.getDriver());
            userDefinedDevice->setScreen(driverConf.getScreen());
        } else {
            userDefinedDevice = *userSearchDefinedDevice;
        }

        auto driverOptions = DRI::Parser::convertSectionsToOptionsObject(driverConf.getSections());

        auto newDeviceApps = userDefinedDevice->getApplications();

        /* Check if the user-defined apps are missing any of the driver option */
        for (auto &userDefinedApp : newDeviceApps) {
            for (const auto &driverDefinedOption : driverOptions) {
                auto optionExists = std::find_if(userDefinedApp->getOptions().begin(),
                                                 userDefinedApp->getOptions().end(),
                                                 [&driverDefinedOption](std::shared_ptr<DRI::ApplicationOption> o) {
                                                     return o->getName() == driverDefinedOption.getName();

                                                 });
                /* Option doesn't exists, lets add it */
                if (optionExists == userDefinedApp->getOptions().end()) {
                    auto newDriverOpt = std::make_shared<DRI::ApplicationOption>();
                    newDriverOpt->setName(driverDefinedOption.getName());
                    newDriverOpt->setValue(driverDefinedOption.getDefaultValue());

                    userDefinedApp->addOption(newDriverOpt);
                }
            }
        }


        /* Check if we can add any of the system-wide apps for this config */
        for (const auto &systemWideApp : systemWideDevice->getApplications()) {
            auto appExists = std::find_if(newDeviceApps.begin(), newDeviceApps.end(),
                                          [&systemWideApp](std::shared_ptr<DRI::Application> app) {
                                              return app->getExecutable() == systemWideApp->getExecutable();
                                          });

            if (appExists == newDeviceApps.end()) {
                auto systemDefinedApp = std::make_shared<DRI::Application>();
                systemDefinedApp->setName(systemWideApp->getName());
                systemDefinedApp->setExecutable(systemWideApp->getExecutable());

                for (const auto &driverOptionObj : driverOptions) {
                    auto optionExists = std::find_if(systemWideApp->getOptions().begin(), systemWideApp->getOptions().end(),
                                                     [&driverOptionObj](std::shared_ptr<DRI::ApplicationOption> a) {
                                                         return driverOptionObj.getName() == a->getName();
                                                     });

                    if (optionExists == systemWideApp->getOptions().end()) {
                        auto newDriverOpt = std::make_shared<DRI::ApplicationOption>();
                        newDriverOpt->setName(driverOptionObj.getName());
                        newDriverOpt->setValue(driverOptionObj.getDefaultValue());

                        systemDefinedApp->addOption(newDriverOpt);
                    } else {
                        /* Option already exists, lets add it */
                        auto newDriverOpt = std::make_shared<DRI::ApplicationOption>();
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
                                       [](std::shared_ptr<DRI::Application> app) {
                                           return app->getExecutable().empty();
                                       });

        if (defaultApp == newDeviceApps.end()) {
            auto defaultApplication = std::make_shared<DRI::Application>();
            defaultApplication->setName("Default");
            auto defaultAppOptions = defaultApplication->getOptions();
            for (auto &driverOptionObj : driverOptions) {
                auto newDriverOpt = std::make_shared<DRI::ApplicationOption>();
                newDriverOpt->setName(driverOptionObj.getName());
                newDriverOpt->setValue(driverOptionObj.getDefaultValue());

                defaultApplication->addOption(newDriverOpt);
            }

            userDefinedDevice->addApplication(defaultApplication);
        }
    }
}