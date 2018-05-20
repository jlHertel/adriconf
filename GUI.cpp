#include "GUI.h"

#include <boost/locale.hpp>
#include "ConfigurationResolver.h"
#include "Writer.h"
#include <fstream>

GUI::GUI() : currentApp(nullptr), currentDriver(nullptr) {
    this->setupLocale();

    /* Load the configurations */
    ConfigurationLoader configurationLoader;
    this->driverConfiguration = configurationLoader.loadDriverSpecificConfiguration(this->locale);
    for (auto &driver : this->driverConfiguration) {
        driver.sortSectionOptions();
    }

    this->systemWideConfiguration = configurationLoader.loadSystemWideConfiguration();
    this->userDefinedConfiguration = configurationLoader.loadUserDefinedConfiguration();
    this->availableGPUs = configurationLoader.loadAvailableGPUs(this->locale);
    this->isPrimeSetup = this->availableGPUs.size() > 1;

    /* For each app setup their prime driver name */
    ConfigurationResolver::updatePrimeApplications(
            this->userDefinedConfiguration,
            this->availableGPUs
    );

    /* Merge all the options in a complete structure */
    ConfigurationResolver::mergeOptionsForDisplay(
            this->systemWideConfiguration,
            this->driverConfiguration,
            this->userDefinedConfiguration,
            this->availableGPUs
    );

    /* Filter invalid options */
    ConfigurationResolver::filterDriverUnsupportedOptions(
            this->driverConfiguration,
            this->userDefinedConfiguration,
            this->availableGPUs
    );

    /* Load the GUI file */
    this->gladeBuilder = Gtk::Builder::create();
    this->gladeBuilder->add_from_resource("/jlHertel/adriconf/DriConf.glade");

    /* Extract the main object */
    this->gladeBuilder->get_widget("mainwindow", this->pWindow);
    if (!pWindow) {
        std::cerr << _("Main window object is not in glade file!") << std::endl;
        return;
    }

    this->pWindow->set_default_size(800, 600);
    this->pWindow->set_size_request(800, 600);

    /* Extract the quit-menu */
    Gtk::ImageMenuItem *pQuitAction;
    this->gladeBuilder->get_widget("quitAction", pQuitAction);
    if (pQuitAction) {
        pQuitAction->signal_activate().connect(sigc::mem_fun(this, &GUI::onQuitPressed));
    }

    /* Extract the save-menu */
    Gtk::ImageMenuItem *pSaveAction;
    this->gladeBuilder->get_widget("saveAction", pSaveAction);
    if (pSaveAction) {
        pSaveAction->signal_activate().connect(sigc::mem_fun(this, &GUI::onSavePressed));
    }

    Glib::RefPtr <Gtk::AccelGroup> accelGroup = this->pWindow->get_accel_group();
    /* Create the menu itens */
    this->pMenuAddApplication = Gtk::manage(new Gtk::MenuItem);
    this->pMenuAddApplication->set_visible(true);
    this->pMenuAddApplication->set_label(_("Add new"));
    this->pMenuAddApplication->signal_activate().connect(sigc::mem_fun(this, &GUI::onAddApplicationPressed));
    this->pMenuAddApplication->add_accelerator(
            "activate",
            accelGroup,
            gdk_keyval_from_name("n"),
            Gdk::ModifierType::CONTROL_MASK,
            Gtk::AccelFlags::ACCEL_MASK
    );

    this->pMenuRemoveApplication = Gtk::manage(new Gtk::MenuItem);
    this->pMenuRemoveApplication->set_visible(true);
    this->pMenuRemoveApplication->set_label(_("Remove current Application"));
    this->pMenuRemoveApplication->signal_activate().connect(sigc::mem_fun(this, &GUI::onRemoveApplicationPressed));
    this->pMenuRemoveApplication->add_accelerator(
            "activate",
            accelGroup,
            gdk_keyval_from_name("d"),
            Gdk::ModifierType::CONTROL_MASK,
            Gtk::AccelFlags::ACCEL_MASK
    );

    /* Extract & generate the menu with the applications */
    this->drawApplicationSelectionMenu();

    /* Draw the final screen */
    this->drawApplicationOptions();

    /* Setup the about dialog */
    this->setupAboutDialog();
}

GUI::~GUI() {
    delete this->pWindow;
}

void GUI::onQuitPressed() {
    if (this->pWindow != nullptr) {
        this->pWindow->hide();
    }
}

void GUI::onSavePressed() {
    std::cout << _("Generating final XML for saving...") << std::endl;
    auto resolvedOptions = ConfigurationResolver::resolveOptionsForSave(
            this->systemWideConfiguration, this->driverConfiguration, this->userDefinedConfiguration,
            this->availableGPUs
    );
    auto rawXML = Writer::generateRawXml(resolvedOptions);
    std::cout << Glib::ustring::compose(_("Writing generated XML: %1"), rawXML) << std::endl;
    std::string userHome(std::getenv("HOME"));
    std::ofstream outFile(userHome + "/.drirc");
    outFile << rawXML;
    outFile.close();
}

Gtk::Window *GUI::getWindowPointer() {
    return this->pWindow;
}

void GUI::setupLocale() {
    boost::locale::generator gen;
    std::locale l = gen("");
    std::locale::global(l);
    std::cout.imbue(l);
    bindtextdomain("adriconf", "/usr/share/locale");
    textdomain("adriconf");

    Glib::ustring langCode(std::use_facet<boost::locale::info>(l).language());

    std::cout << Glib::ustring::compose(_("Current language code is %1"), langCode) << std::endl;

    this->locale = langCode;
}

void GUI::drawApplicationSelectionMenu() {
    Gtk::Menu *pApplicationMenu;
    this->gladeBuilder->get_widget("ApplicationMenu", pApplicationMenu);

    if (pApplicationMenu) {
        /* Remove any item already defined */
        for (auto &menuItem : pApplicationMenu->get_children()) {
            pApplicationMenu->remove(*menuItem);
        }

        /* Clear the items already selected */
        this->currentDriver = nullptr;
        /* No need to set the current application executables, as the default doesn't have one */
        this->currentApp = nullptr;

        /* Sort the applications to maintain a good human GUI */
        for (auto &driver : this->userDefinedConfiguration) {
            driver->sortApplications();
        }

        /* Add the actions of add/remove apps */
        pApplicationMenu->add(*this->pMenuAddApplication);
        pApplicationMenu->add(*this->pMenuRemoveApplication);

        Gtk::RadioButtonGroup appRadioGroup;
        bool groupInitialized = false;

        for (auto &driver : this->userDefinedConfiguration) {

            if (this->currentDriver == nullptr) {
                // Locate the driver config
                auto foundDriver = std::find_if(this->driverConfiguration.begin(), this->driverConfiguration.end(),
                                                [driver](const DriverConfiguration &d) {
                                                    return d.getDriverName() == driver->getDriver();
                                                });
                if (foundDriver == this->driverConfiguration.end()) {
                    std::cerr << Glib::ustring::compose(_("Driver %1 not found"), driver) << std::endl;
                }
                this->currentDriver = &(*foundDriver);
            }

            Gtk::MenuItem *driverMenuItem = Gtk::manage(new Gtk::MenuItem);
            driverMenuItem->set_visible(true);
            driverMenuItem->set_label(driver->getDriver());

            Gtk::Menu *driverSubMenu = Gtk::manage(new Gtk::Menu);
            driverSubMenu->set_visible(true);
            for (auto &possibleApp : driver->getApplications()) {
                Gtk::RadioMenuItem *appMenuItem = Gtk::manage(new Gtk::RadioMenuItem);
                appMenuItem->set_visible(true);
                appMenuItem->set_label(possibleApp->getName());

                if (!groupInitialized) {
                    appRadioGroup = appMenuItem->get_group();
                    groupInitialized = true;
                } else {
                    appMenuItem->set_group(appRadioGroup);
                }

                if (this->currentDriver->getDriverName() == driver->getDriver() &&
                    possibleApp->getExecutable().empty()) {
                    appMenuItem->set_active(true);

                    this->currentApp = possibleApp;
                }

                appMenuItem->signal_toggled().connect(sigc::bind<Glib::ustring, Glib::ustring>(
                        sigc::mem_fun(this, &GUI::onApplicationSelected),
                        driver->getDriver(), possibleApp->getExecutable()
                ));

                driverSubMenu->append(*appMenuItem);
            }

            driverMenuItem->set_submenu(*driverSubMenu);

            pApplicationMenu->add(*driverMenuItem);
        }
    }
}

void GUI::onApplicationSelected(const Glib::ustring driverName, const Glib::ustring applicationName) {
    if (driverName == this->currentDriver->getDriverName() && applicationName == this->currentApp->getExecutable()) {
        return;
    }

    /* Find the application */
    auto userSelectedDriver = std::find_if(this->userDefinedConfiguration.begin(), this->userDefinedConfiguration.end(),
                                           [driverName](Device_ptr device) {
                                               return driverName == device->getDriver();
                                           }
    );
    auto selectedApp = std::find_if((*userSelectedDriver)->getApplications().begin(),
                                    (*userSelectedDriver)->getApplications().end(),
                                    [applicationName](Application_ptr app) {
                                        return applicationName == app->getExecutable();
                                    }
    );

    if (selectedApp == (*userSelectedDriver)->getApplications().end()) {
        std::cerr << Glib::ustring::compose(_("Application %1 not found "), applicationName)
                  << std::endl;
        return;
    }

    this->currentApp = *selectedApp;

    auto driverSelected = std::find_if(this->driverConfiguration.begin(), this->driverConfiguration.end(),
                                       [driverName](const DriverConfiguration &d) {
                                           return d.getDriverName() == driverName;
                                       });

    if (driverSelected == this->driverConfiguration.end()) {
        std::cerr << Glib::ustring::compose(_("Driver %1 not found "), driverName) << std::endl;
        return;
    }

    this->currentDriver = &(*driverSelected);

    this->drawApplicationOptions();
}

void GUI::drawApplicationOptions() {
    auto selectedAppOptions = this->currentApp->getOptions();

    /* Get the notebook itself */
    Gtk::Notebook *pNotebook;
    this->gladeBuilder->get_widget("notebook", pNotebook);
    if (!pNotebook) {
        std::cerr << _("Notebook object not found in glade file!") << std::endl;
        return;
    }

    /* Remove any previous defined page */
    int numberOfPages = pNotebook->get_n_pages();

    for (int i = 0; i < numberOfPages; i++) {
        pNotebook->remove_page(-1);
    }

    /* Remove any previous defined comboBox */
    this->currentComboBoxes.clear();
    /* Remove any previous defined spinButton */
    this->currentSpinButtons.clear();

    pNotebook->set_visible(true);

    std::list<Section> definedSections;
    if (this->currentApp->getIsUsingPrime()) {
        definedSections = this->availableGPUs[this->currentApp->getDevicePCIId()]->getSections();
    } else {
        definedSections = this->currentDriver->getSections();
    }

    /* Draw each section as a tab */
    for (auto &section : definedSections) {

        Gtk::Box *tabBox = Gtk::manage(new Gtk::Box);
        tabBox->set_visible(true);
        tabBox->set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
        tabBox->set_margin_start(8);
        tabBox->set_margin_end(8);
        tabBox->set_margin_top(10);


        /* Draw each field individually */
        for (auto &option : section.getOptions()) {
            auto optionValue = std::find_if(selectedAppOptions.begin(), selectedAppOptions.end(),
                                            [&option](ApplicationOption_ptr o) {
                                                return option.getName() == o->getName();
                                            });

            if (optionValue == selectedAppOptions.end()) {
                std::cerr << Glib::ustring::compose(
                        _("Option %1 doesn't exist in application %2. Merge failed"),
                        option.getName(),
                        this->currentApp->getName()
                ) << std::endl;
                return;
            }

            Gtk::Box *optionBox = Gtk::manage(new Gtk::Box);
            optionBox->set_visible(true);
            optionBox->set_orientation(Gtk::Orientation::ORIENTATION_HORIZONTAL);
            optionBox->set_margin_bottom(10);

            if (option.getType() == "bool") {
                Gtk::Switch *optionSwitch = Gtk::manage(new Gtk::Switch);
                optionSwitch->set_visible(true);

                if ((*optionValue)->getValue() == "true") {
                    optionSwitch->set_active(true);
                }

                optionSwitch->property_active().signal_changed().connect(sigc::bind<Glib::ustring>(
                        sigc::mem_fun(this, &GUI::onCheckboxChanged), option.getName()
                ));

                optionBox->pack_end(*optionSwitch, false, false);
            }

            if (option.isFakeBool()) {
                Gtk::Switch *optionSwitch = Gtk::manage(new Gtk::Switch);
                optionSwitch->set_visible(true);

                if ((*optionValue)->getValue() == "1") {
                    optionSwitch->set_active(true);
                }

                optionSwitch->property_active().signal_changed().connect(sigc::bind<Glib::ustring>(
                        sigc::mem_fun(this, &GUI::onFakeCheckBoxChanged), option.getName()
                ));

                optionBox->pack_end(*optionSwitch, false, false);
            }

            if (option.getType() == "enum" && !option.isFakeBool()) {
                Gtk::ComboBox *optionCombo = Gtk::manage(new Gtk::ComboBox);
                optionCombo->set_visible(true);

                Glib::RefPtr <Gtk::ListStore> listStore = Gtk::ListStore::create(this->comboColumns);
                optionCombo->set_model(listStore);


                int counter = 0;
                for (auto const &enumOption : option.getEnumValues()) {
                    Gtk::TreeModel::Row row = *(listStore->append());
                    row[this->comboColumns.optionName] = enumOption.first;
                    row[this->comboColumns.optionValue] = enumOption.second;

                    if (enumOption.second == (*optionValue)->getValue()) {
                        optionCombo->set_active(row);
                    }
                    counter++;
                }

                optionCombo->pack_start(this->comboColumns.optionName);

                optionCombo->signal_changed().connect(sigc::bind<Glib::ustring>(
                        sigc::mem_fun(this, &GUI::onComboboxChanged), option.getName()
                ));

                this->currentComboBoxes[option.getName()] = optionCombo;

                optionBox->pack_end(*optionCombo, false, false);
            }

            if (option.getType() == "int") {
                Gtk::SpinButton *optionEntry = Gtk::manage(new Gtk::SpinButton);
                optionEntry->set_visible(true);

                auto currentValue = (*optionValue)->getValue();

                auto adjustment = Gtk::Adjustment::create(
                        std::stof(currentValue),
                        option.getValidValueStart(),
                        option.getValidValueEnd(),
                        1,
                        10
                );

                optionEntry->set_adjustment(adjustment);
                optionEntry->signal_changed().connect(sigc::bind<Glib::ustring>(
                        sigc::mem_fun(this, &GUI::onNumberEntryChanged), option.getName()
                ));

                this->currentSpinButtons[option.getName()] = optionEntry;

                optionBox->pack_end(*optionEntry, false, true);
            }

            Gtk::Label *label = Gtk::manage(new Gtk::Label);
            label->set_label(option.getDescription());
            label->set_visible(true);
            label->set_justify(Gtk::Justification::JUSTIFY_LEFT);
            label->set_line_wrap(true);
            label->set_margin_start(10);
            optionBox->pack_start(*label, false, true);

            tabBox->add(*optionBox);
        }


        Gtk::ScrolledWindow *scrolledWindow = Gtk::manage(new Gtk::ScrolledWindow);
        scrolledWindow->set_visible(true);
        scrolledWindow->add(*tabBox);

        pNotebook->append_page(*scrolledWindow, section.getDescription());
    }

    /* If we have more than one GPU then we are under PRIME */
    if (this->availableGPUs.size() > 1) {
        Gtk::Box *primeTabBox = Gtk::manage(new Gtk::Box);
        primeTabBox->set_visible(true);
        primeTabBox->set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
        primeTabBox->set_margin_start(8);
        primeTabBox->set_margin_end(8);
        primeTabBox->set_margin_top(10);

        Gtk::ComboBox *gpuCombo = Gtk::manage(new Gtk::ComboBox);
        gpuCombo->set_visible(true);

        Glib::RefPtr <Gtk::ListStore> listStore = Gtk::ListStore::create(this->comboColumns);
        gpuCombo->set_model(listStore);

        Gtk::TreeModel::Row firstRow = *(listStore->append());
        firstRow[this->comboColumns.optionName] = _("Use default GPU of screen");
        firstRow[this->comboColumns.optionValue] = "";

        if (!this->currentApp->getIsUsingPrime()) {
            gpuCombo->set_active(firstRow);
        }

        for (auto const &enumOption : this->availableGPUs) {
            Gtk::TreeModel::Row row = *(listStore->append());
            row[this->comboColumns.optionName] = enumOption.second->getDeviceName();
            row[this->comboColumns.optionValue] = enumOption.second->getPciId();

            if (this->currentApp->getDevicePCIId() == enumOption.second->getPciId()) {
                gpuCombo->set_active(row);
            }
        }

        gpuCombo->pack_start(this->comboColumns.optionName);

        gpuCombo->signal_changed().connect(sigc::bind<Glib::ustring>(
                sigc::mem_fun(this, &GUI::onComboboxChanged), "device_id"
        ));

        this->currentComboBoxes["device_id"] = gpuCombo;


        Gtk::Label *label = Gtk::manage(new Gtk::Label);
        label->set_label(_("Force Application to use GPU"));
        label->set_visible(true);
        label->set_justify(Gtk::Justification::JUSTIFY_LEFT);
        label->set_line_wrap(true);
        label->set_margin_start(10);

        Gtk::Box *optionBox = Gtk::manage(new Gtk::Box);
        optionBox->set_visible(true);
        optionBox->set_orientation(Gtk::Orientation::ORIENTATION_HORIZONTAL);
        optionBox->set_margin_bottom(10);

        optionBox->pack_end(*gpuCombo, false, false);
        optionBox->pack_start(*label, false, true);
        primeTabBox->add(*optionBox);

        pNotebook->append_page(*primeTabBox, _("PRIME Settings"));
    }

}

void GUI::onCheckboxChanged(Glib::ustring optionName) {
    auto eventSelectedAppOptions = this->currentApp->getOptions();

    auto currentOption = std::find_if(eventSelectedAppOptions.begin(), eventSelectedAppOptions.end(),
                                      [&optionName](const ApplicationOption_ptr &a) {
                                          return a->getName() == optionName;
                                      });

    if ((*currentOption)->getValue() == "true") {
        (*currentOption)->setValue("false");
    } else {
        (*currentOption)->setValue("true");
    }
}

void GUI::onFakeCheckBoxChanged(Glib::ustring optionName) {
    auto eventSelectedAppOptions = this->currentApp->getOptions();

    auto currentOption = std::find_if(eventSelectedAppOptions.begin(), eventSelectedAppOptions.end(),
                                      [&optionName](const ApplicationOption_ptr &a) {
                                          return a->getName() == optionName;
                                      });

    if ((*currentOption)->getValue() == "1") {
        (*currentOption)->setValue("0");
    } else {
        (*currentOption)->setValue("1");
    }
}

void GUI::onComboboxChanged(Glib::ustring optionName) {
    auto eventSelectedAppOptions = this->currentApp->getOptions();

    auto currentOptionPtr = std::find_if(eventSelectedAppOptions.begin(), eventSelectedAppOptions.end(),
                                         [&optionName](const ApplicationOption_ptr &a) {
                                             return a->getName() == optionName;
                                         });

    ApplicationOption_ptr currentOption = nullptr;

    /* If the PRIME option doesn't exist, lets add it */
    if (optionName == "device_id" && currentOptionPtr == eventSelectedAppOptions.end()) {
        ApplicationOption_ptr primeOpt = std::make_shared<ApplicationOption>();
        primeOpt->setName("device_id");
        this->currentApp->addOption(primeOpt);

        currentOption = primeOpt;
    } else {
        currentOption = *currentOptionPtr;
    }

    Gtk::TreeModel::iterator iter = this->currentComboBoxes[optionName]->get_active();
    if (iter) {
        Gtk::TreeModel::Row selectedRow = *iter;

        currentOption->setValue(selectedRow[comboColumns.optionValue]);


        if (optionName == "device_id") {
            /* Reset to default */
            if (selectedRow[comboColumns.optionValue] == "") {
                this->currentApp->setIsUsingPrime(false);
                this->currentApp->setDevicePCIId("");
            } else {
                /* Set this app as a prime-enabled app */
                this->currentApp->setPrimeDriverName(
                        this->availableGPUs[selectedRow[comboColumns.optionValue]]->getDriverName()
                );
                this->currentApp->setIsUsingPrime(true);
                this->currentApp->setDevicePCIId(selectedRow[comboColumns.optionValue]);

                /* Add the missing options of the new driver */
                auto newDriverOptions = this->availableGPUs[selectedRow[comboColumns.optionValue]]->getOptionsMap();
                ConfigurationResolver::addMissingDriverOptions(this->currentApp, newDriverOptions);

                /* TODO: We should trigget a full-redraw of the available options, but how? */
            }
        }
    }
}

void GUI::onNumberEntryChanged(Glib::ustring optionName) {
    auto eventSelectedAppOptions = this->currentApp->getOptions();

    auto currentOption = std::find_if(eventSelectedAppOptions.begin(), eventSelectedAppOptions.end(),
                                      [&optionName](ApplicationOption_ptr a) {
                                          return a->getName() == optionName;
                                      });

    auto enteredValue = this->currentSpinButtons[optionName]->get_value();
    Glib::ustring
    enteredValueStr(std::to_string((int) enteredValue));
    (*currentOption)->setValue(enteredValueStr);
}

void GUI::setupAboutDialog() {
    this->aboutDialog.set_transient_for(*this->pWindow);
    this->aboutDialog.set_program_name("Advanced DRI Configurator");
    this->aboutDialog.set_version("1.0.0");
    this->aboutDialog.set_copyright("Jean Lorenz Hertel");
    this->aboutDialog.set_comments(_("An advanced DRI configurator tool."));
    this->aboutDialog.set_license("GPLv3");

    this->aboutDialog.set_website("https://github.com/jlHertel/adriconf");
    this->aboutDialog.set_website_label(_("Source Code"));

    std::vector<Glib::ustring> list_authors;
    list_authors.emplace_back("Jean Lorenz Hertel");
    this->aboutDialog.set_authors(list_authors);

    this->aboutDialog.signal_response().connect([this](int responseCode) {
        switch (responseCode) {
            case Gtk::RESPONSE_CLOSE:
            case Gtk::RESPONSE_CANCEL:
            case Gtk::RESPONSE_DELETE_EVENT:
                this->aboutDialog.hide();
                break;
            default:
                std::cout << Glib::ustring::compose(_("Unexpected response code from about dialog: %1"), responseCode)
                          << std::endl;
                break;
        }
    });

    Gtk::ImageMenuItem *pAboutAction;
    this->gladeBuilder->get_widget("aboutAction", pAboutAction);
    if (pAboutAction) {
        pAboutAction->signal_activate().connect([this]() {
            this->aboutDialog.show();
        });
    }
}

void GUI::onRemoveApplicationPressed() {
    if (this->currentApp->getExecutable().empty()) {
        Gtk::MessageDialog dialog(*(this->pWindow), _("The default application cannot be removed."));
        dialog.set_secondary_text(_("The driver needs a default configuration."));
        dialog.run();
        return;
    }

    for (auto &device : this->userDefinedConfiguration) {
        if (device->getDriver() == this->currentDriver->getDriverName()) {
            device->getApplications().remove_if([this](const Application_ptr &app) {
                return app->getExecutable() == this->currentApp->getExecutable();
            });
        }
    }

    Gtk::MessageDialog dialog(*(this->pWindow), _("Application removed successfully."));
    dialog.set_secondary_text(_("The application has been removed."));
    dialog.run();

    this->drawApplicationSelectionMenu();
    this->drawApplicationOptions();
}

void GUI::onAddApplicationPressed() {
    Gtk::Dialog *pDialog;
    Gtk::Entry *pAppName;
    Gtk::Entry *pAppExecutable;
    Gtk::ComboBoxText *pAppDriver;

    this->gladeBuilder->get_widget("newAppDialog", pDialog);
    if (!pDialog) {
        std::cerr << _("Add Application dialog is not in glade file!") << std::endl;
        return;
    }

    this->gladeBuilder->get_widget("newAppName", pAppName);
    if (!pAppName) {
        std::cerr << _("Add Application app name widget is not in glade file!") << std::endl;
        return;
    }

    this->gladeBuilder->get_widget("newAppExecutable", pAppExecutable);
    if (!pAppExecutable) {
        std::cerr << _("Add Application app executable widget is not in glade file!") << std::endl;
        return;
    }

    this->gladeBuilder->get_widget("newAppDriver", pAppDriver);
    if (!pAppDriver) {
        std::cerr << _("Add Application app driver widget is not in glade file!") << std::endl;
        return;
    }

    /* Reset all widgets */
    pAppName->set_text("");
    pAppExecutable->set_text("");
    pAppDriver->remove_all();

    for (const auto &driverConfig : this->driverConfiguration) {
        pAppDriver->append(driverConfig.getDriverName());
    }

    pAppDriver->set_active(0);

    int result = pDialog->run();
    pDialog->hide();

    if (result == 50) {
        /* Check the given information and try to save the app */
        if (pAppName->get_text().empty() ||
            pAppExecutable->get_text().empty() ||
            pAppDriver->get_active_text().empty()) {
            Gtk::MessageDialog validationDialog(*(this->pWindow), _("Validation error"));
            validationDialog.set_secondary_text(
                    _("You need to specify the application name, executable and driver."));
            validationDialog.run();
            return;
        }

        Application_ptr newApplication;

        for (const auto &driver : this->driverConfiguration) {
            if (driver.getDriverName() == pAppDriver->get_active_text()) {
                newApplication = driver.generateApplication();
            }
        }

        newApplication->setName(pAppName->get_text());
        newApplication->setExecutable(pAppExecutable->get_text());

        for (auto &userConfig : this->userDefinedConfiguration) {
            if (userConfig->getDriver() == pAppDriver->get_active_text()) {
                userConfig->addApplication(newApplication);
            }
        }

        Gtk::MessageDialog dialog(*(this->pWindow), _("Application successfully added."));
        dialog.set_secondary_text(_("The application was successfully added. Reloading default app options."));

        dialog.run();

        this->drawApplicationSelectionMenu();
        this->drawApplicationOptions();
    }
}
