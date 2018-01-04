#include "ConfigurationResolver.h"
#include "Parser.h"
#include <glibmm/i18n.h>

/* This function will remove unnecessary options/applications and generate a structure with only the necessary items */
std::list<DRI::Device> DRI::ConfigurationResolver::resolveOptionsForSave(
        const DRI::Device &systemWideDevice,
        const std::list<DRI::DriverConfiguration> &driverAvailableOptions,
        std::list<DRI::Device> userDefinedOptions
) {
    /* Create the final driverList */
    std::list<DRI::Device> mergedDevices;

    /* Precedence: userDefined > System Wide > Driver Default */
    for (auto &userDevice : userDefinedOptions) {
        DRI::Device mergedDevice;

        mergedDevice.setDriver(userDevice.getDriver());
        mergedDevice.setScreen(userDevice.getScreen());

        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDevice](const DRI::DriverConfiguration &d) {
                                             return d.getScreen() == userDevice.getScreen();
                                         });

        std::list<DRI::Option> driverOptions;

        if (driverConfig != driverAvailableOptions.end()) {
            driverOptions = DRI::Parser::convertSectionsToOptionsObject(driverConfig->getSections());
        }

        for (auto const &userApplication : userDevice.getApplications()) {
            DRI::Application mergedApp;
            /* If this application already exists systemWide, we need to do a merge on it */
            if (systemWideDevice.applicationExists(userApplication.getExecutable())) {
                auto systemWideApp = systemWideDevice.findApplication(userApplication.getExecutable());

                bool addApplication = false;

                /* List of name-value options */
                auto systemWideOptions = systemWideApp.getOptions();
                std::map<Glib::ustring, Glib::ustring> mergedOptions;

                for (auto const &option : userApplication.getOptions()) {
                    auto systemWideOption = systemWideOptions.find(option.first);

                    if (systemWideOption != systemWideOptions.end()) {
                        /* If the option set is the same as the one used just ignore this options*/
                        if (systemWideOption->second != option.second) {
                            addApplication = true;
                            mergedOptions.emplace(option.first, option.second);
                        } else {
                            std::cout << Glib::ustring::compose(
                                    _("Option '%1' from application '%2' on driver '%3' and screen '%4' is the same as the system wide value (%5). Option will be ignored."),
                                    option.first,
                                    userApplication.getName(),
                                    userDevice.getDriver(),
                                    userDevice.getScreen(),
                                    option.second
                            ) << std::endl;
                        }

                    } else {
                        /*
                         * Option doesn't exist in system-wide
                         * We must check what is the default value from driver
                         */

                        auto driverOption = std::find_if(driverOptions.begin(), driverOptions.end(),
                                                         [&option](DRI::Option &driverDefinedOption) {
                                                             return driverDefinedOption.getName() == option.first;
                                                         });

                        if (driverOption->getDefaultValue() == option.second) {
                            std::cout << Glib::ustring::compose(
                                    _("Option \"%1\" on driver \"%2\" and screen \"%3\" is the same as driver default (%4). Option will be ignored."),
                                    option.first,
                                    userDevice.getDriver(),
                                    userDevice.getScreen(),
                                    driverOption->getDefaultValue()
                            ) << std::endl;
                        } else {
                            addApplication = true;
                            mergedOptions.emplace(option.first, option.second);
                        }
                    }

                }

                if (addApplication) {
                    mergedApp.setOptions(mergedOptions);
                    mergedApp.setExecutable(userApplication.getExecutable());
                    mergedApp.setName(userApplication.getName());
                    mergedDevice.addApplication(mergedApp);
                } else {
                    std::cout << Glib::ustring::compose(
                            _("Application '%1' on driver '%2' and screen '%3' has all the values already defined in system wide configuration. Application will be ignored."),
                            userApplication.getName(),
                            mergedDevice.getDriver(),
                            mergedDevice.getScreen()
                    ) << std::endl;
                }

            } else {
                /**
                 * Application doesn't exist in system-wide configuration
                 * but we must check each option to see if its value is the same as the driver default
                 */
                std::map<Glib::ustring, Glib::ustring> mergedOptions;

                bool addApplication = false;

                for (auto &userDefinedOption : userApplication.getOptions()) {
                    auto driverOption = std::find_if(driverOptions.begin(), driverOptions.end(),
                                                     [&userDefinedOption](DRI::Option &driverDefinedOption) {
                                                         return driverDefinedOption.getName() ==
                                                                userDefinedOption.first;
                                                     });

                    if (driverOption->getDefaultValue() == userDefinedOption.second) {
                        std::cout << Glib::ustring::compose(
                                _("Option \"%1\" on driver \"%2\" and screen \"%3\" is the same as driver default (%4). Option will be ignored."),
                                userDefinedOption.first,
                                userDevice.getDriver(),
                                userDevice.getScreen(),
                                driverOption->getDefaultValue()
                        ) << std::endl;
                    } else {
                        addApplication = true;
                        mergedOptions.emplace(userDefinedOption.first, userDefinedOption.second);
                    }
                }

                if(addApplication) {
                    mergedApp.setExecutable(userApplication.getExecutable());
                    mergedApp.setName(userApplication.getName());
                    mergedApp.setOptions(mergedOptions);

                    mergedDevice.addApplication(mergedApp);
                } else {
                    std::cout << Glib::ustring::compose(
                            _("Application '%1' on driver '%2' and screen '%3' has all the values already defined in driver default configuration. Application will be ignored."),
                            userApplication.getName(),
                            mergedDevice.getDriver(),
                            mergedDevice.getScreen()
                    ) << std::endl;
                }
            }
        }

        mergedDevices.emplace_back(mergedDevice);
    }

    return mergedDevices;
}

std::list<DRI::Device> DRI::ConfigurationResolver::filterDriverUnsupportedOptions(
        std::list<DRI::DriverConfiguration> driverAvailableOptions,
        std::list<DRI::Device> userDefinedOptions
) {
    for (auto &userDevice : userDefinedOptions) {
        auto driverConfig = std::find_if(driverAvailableOptions.begin(), driverAvailableOptions.end(),
                                         [&userDevice](const DRI::DriverConfiguration &d) {
                                             return d.getScreen() == userDevice.getScreen();
                                         });

        std::list<Glib::ustring> driverOptions;

        if (driverConfig != driverAvailableOptions.end()) {
            driverOptions = DRI::Parser::convertSectionsToOptions(driverConfig->getSections());
        }

        auto applications = userDevice.getApplications();
        for (auto &app : applications) {
            auto options = app.getOptions();

            auto itr = options.begin();
            while (itr != options.end()) {
                auto driverSupports = std::find(driverOptions.begin(), driverOptions.end(), itr->first);

                if (driverSupports == driverOptions.end()) {
                    std::cerr << Glib::ustring::compose(
                            _("Driver '%1' at screen '%2' doesn't support option '%3' on application '%4'. Option will be removed."),
                            driverConfig->getDriver(),
                            driverConfig->getScreen(),
                            itr->first,
                            app.getName()
                    ) << std::endl;
                    itr = options.erase(itr);
                } else {
                    ++itr;
                }
            }

            app.setOptions(options);
        }

        userDevice.setApplications(applications);
    }

    return userDefinedOptions;
}