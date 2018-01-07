#include "GUI.h"

#include <boost/locale.hpp>
#include "Parser.h"
#include "ConfigurationResolver.h"
#include "DRIQuery.h"
#include "Writer.h"
#include "DriverConfiguration.h"

#include <iostream>
#include <fstream>

DRI::GUI::GUI() : currentApp(nullptr) {
    this->setupLocale();

    /* Load the configurations */
    DRI::ConfigurationLoader configurationLoader;
    this->driverConfiguration = configurationLoader.loadDriverSpecificConfiguration(this->locale);
    this->systemWideConfiguration = configurationLoader.loadSystemWideConfiguration();
    this->userDefinedConfiguration = configurationLoader.loadUserDefinedConfiguration();

    /* Merge all the options in a complete structure */
    DRI::ConfigurationResolver::mergeOptionsForDisplay(
            this->systemWideConfiguration,
            this->driverConfiguration,
            this->userDefinedConfiguration
    );

    /* Filter invalid options */
    DRI::ConfigurationResolver::filterDriverUnsupportedOptions(
            this->driverConfiguration,
            this->userDefinedConfiguration
    );

    /* Load the GUI file */
    this->gladeBuilder = Gtk::Builder::create();
    this->gladeBuilder->add_from_file("DriConf.glade");

    /* Extract the main object */
    this->gladeBuilder->get_widget("mainwindow", this->pWindow);
    if (!pWindow) {
        std::cerr << _("Main window object is not in glade file!") << std::endl;
        return;
    }

    this->pWindow->set_default_size(800, 600);

    /* Extract the quit-menu */
    Gtk::ImageMenuItem *pQuitAction;
    this->gladeBuilder->get_widget("quitAction", pQuitAction);
    if (pQuitAction) {
        pQuitAction->signal_activate().connect(sigc::mem_fun(this, &DRI::GUI::onQuitPressed));
    }

    /* Extract the save-menu */
    Gtk::ImageMenuItem *pSaveAction;
    this->gladeBuilder->get_widget("saveAction", pSaveAction);
    if (pSaveAction) {
        pSaveAction->signal_activate().connect(sigc::mem_fun(this, &DRI::GUI::onSavePressed));
    }

    /* Extract & generate the menu with the applications */
    this->drawApplicationSelectionMenu();

    /* Draw the final screen */
    this->drawApplicationOptions();
}

DRI::GUI::~GUI() {
    delete this->pWindow;
}

void DRI::GUI::onQuitPressed() {
    if (this->pWindow != nullptr) {
        this->pWindow->hide();
    }
}

void DRI::GUI::onSavePressed() {
    std::cout << _("Generating final XML for saving...") << std::endl;
    auto resolvedOptions = DRI::ConfigurationResolver::resolveOptionsForSave(
            this->systemWideConfiguration, this->driverConfiguration, this->userDefinedConfiguration
    );
    auto rawXML = DRI::Writer::generateRawXml(resolvedOptions);
    std::cout << Glib::ustring::compose(_("Writing generated XML: %1"), rawXML) << std::endl;
    std::string userHome(std::getenv("HOME"));
    std::ofstream outFile(userHome + "/.drirc");
    outFile << rawXML;
    outFile.close();
}

Gtk::Window *DRI::GUI::getWindowPointer() {
    return this->pWindow;
}

void DRI::GUI::setupLocale() {
    boost::locale::generator gen;
    std::locale l = gen("");
    std::locale::global(l);
    std::cout.imbue(l);
    Glib::ustring langCode(std::use_facet<boost::locale::info>(l).language());

    std::cout << Glib::ustring::compose(_("Current language code is %1"), langCode) << std::endl;

    this->locale = langCode;
}

void DRI::GUI::drawApplicationSelectionMenu() {
    Gtk::Menu *pApplicationMenu;
    this->gladeBuilder->get_widget("ApplicationMenu", pApplicationMenu);

    if (pApplicationMenu) {
        /* Remove any item already defined */
        for (auto &menuItem : pApplicationMenu->get_children()) {
            pApplicationMenu->remove(*menuItem);
        }

        /* Clear the items already selected */
        this->currentSelectedDriver = "";
        /* No need to set the current application executables, as the default doesn't have one */
        this->currentSelectedApplication = "";

        /* Sort the applications to maintain a good human GUI */
        for (auto &driver : this->userDefinedConfiguration) {
            driver->sortApplications();
        }

        Gtk::RadioButtonGroup appRadioGroup;
        bool groupInitialized = false;

        for (auto &driver : this->userDefinedConfiguration) {

            if (this->currentSelectedDriver.empty()) {
                this->currentSelectedDriver = driver->getDriver();

                // Locate the driver config
                auto foundDriver = std::find_if(this->driverConfiguration.begin(), this->driverConfiguration.end(),
                                                [this](DRI::DriverConfiguration d) {
                                                    return d.getDriver() == this->currentSelectedDriver;
                                                });
                if (foundDriver == this->driverConfiguration.end()) {
                    std::cerr << Glib::ustring::compose(_("Driver %1 not found"), this->currentSelectedDriver)
                              << std::endl;
                }
                this->currentDriver = *foundDriver;
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

                if (this->currentSelectedDriver == driver->getDriver() && possibleApp->getExecutable().empty()) {
                    appMenuItem->set_active(true);

                    this->currentApp = possibleApp;
                }

                appMenuItem->signal_toggled().connect(sigc::bind<Glib::ustring, Glib::ustring>(
                        sigc::mem_fun(this, &DRI::GUI::onApplicationSelected),
                        driver->getDriver(), possibleApp->getExecutable()
                ));

                driverSubMenu->append(*appMenuItem);
            }

            driverMenuItem->set_submenu(*driverSubMenu);

            pApplicationMenu->add(*driverMenuItem);
        }
    }
}

void DRI::GUI::onApplicationSelected(const Glib::ustring driverName, const Glib::ustring applicationName) {
    if (driverName == this->currentSelectedDriver && applicationName == this->currentSelectedApplication) {
        return;
    }

    this->currentSelectedDriver = driverName;
    this->currentSelectedApplication = applicationName;

    /* Find the application */
    auto userSelectedDriver = std::find_if(this->userDefinedConfiguration.begin(), this->userDefinedConfiguration.end(),
                                           [this](std::shared_ptr<DRI::Device> device) {
                                               return this->currentSelectedDriver == device->getDriver();
                                           }
    );
    auto selectedApp = std::find_if((*userSelectedDriver)->getApplications().begin(),
                                    (*userSelectedDriver)->getApplications().end(),
                                    [this](std::shared_ptr<DRI::Application> app) {
                                        return this->currentSelectedApplication == app->getExecutable();
                                    }
    );

    if (selectedApp == (*userSelectedDriver)->getApplications().end()) {
        std::cerr << Glib::ustring::compose(_("Application %1 not found "), this->currentSelectedApplication)
                  << std::endl;
        return;
    }

    this->currentApp = *selectedApp;

    auto driverSelected = std::find_if(this->driverConfiguration.begin(), this->driverConfiguration.end(),
                                       [this](const DRI::DriverConfiguration &d) {
                                           return d.getDriver() == this->currentSelectedDriver;
                                       });

    if (driverSelected == this->driverConfiguration.end()) {
        std::cerr << Glib::ustring::compose(_("Driver %1 not found "), this->currentSelectedDriver) << std::endl;
        return;
    }

    this->currentDriver = *driverSelected;

    this->drawApplicationOptions();
}

void DRI::GUI::drawApplicationOptions() {
    auto selectedAppOptions = this->currentApp->getOptions();

    /* Get the notebook itself */
    Gtk::Notebook *pNotebook;
    this->gladeBuilder->get_widget("notebook", pNotebook);
    if (!pNotebook) {
        std::cerr << _("Notebook object not found in glade file!") << std::endl;
        return;
    }

    pNotebook->set_visible(true);

    /* Draw each section as a tab */
    for (auto &section : this->currentDriver.getSections()) {
        Gtk::Box *tabBox = Gtk::manage(new Gtk::Box);
        tabBox->set_visible(true);
        tabBox->set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);

        /* Draw each field individually */
        for (auto &option : section.getOptions()) {
            auto optionValue = std::find_if(selectedAppOptions.begin(), selectedAppOptions.end(),
                                            [&option](std::shared_ptr<DRI::ApplicationOption> o) {
                                                return option.getName() == o->getName();
                                            });

            if(optionValue == selectedAppOptions.end()) {
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

            Gtk::Label *label = Gtk::manage(new Gtk::Label);
            label->set_label(option.getName());
            label->set_visible(true);
            optionBox->add(*label);

            if (option.getType() == "bool") {
                Gtk::CheckButton *optionCheckButton = Gtk::manage(new Gtk::CheckButton);
                optionCheckButton->set_visible(true);

                if ((*optionValue)->getValue() == "true") {
                    optionCheckButton->set_active(true);
                }

                optionCheckButton->signal_clicked().connect(sigc::bind<Glib::ustring>(
                        sigc::mem_fun(this, &DRI::GUI::onCheckboxChanged), option.getName()
                ));

                optionBox->add(*optionCheckButton);
            }

            tabBox->add(*optionBox);
        }


        /* Connect the field directly to the app object */

        pNotebook->append_page(*tabBox, section.getDescription());
    }
}

void DRI::GUI::onCheckboxChanged(Glib::ustring optionName) {
    /* Find the application itself */
    auto eventSelectedDriver = std::find_if(this->userDefinedConfiguration.begin(),
                                            this->userDefinedConfiguration.end(),
                                            [this](std::shared_ptr<DRI::Device> device) {
                                                return this->currentSelectedDriver == device->getDriver();
                                            }
    );
    auto eventSelectedApp = std::find_if((*eventSelectedDriver)->getApplications().begin(),
                                         (*eventSelectedDriver)->getApplications().end(),
                                         [this](std::shared_ptr<DRI::Application> app) {
                                             return this->currentSelectedApplication ==
                                                    app->getExecutable();
                                         }
    );

    auto eventSelectedAppOptions = (*eventSelectedApp)->getOptions();

    std::cout << "Called signal for option " << optionName << std::endl;

    auto currentOption = std::find_if(eventSelectedAppOptions.begin(), eventSelectedAppOptions.end(),
                                      [&optionName](std::shared_ptr<DRI::ApplicationOption> a) {
                                          return a->getName() == optionName;
                                      });

    if ((*currentOption)->getValue() == "true") {
        std::cout << "Setting value to false" << std::endl;
        (*currentOption)->setValue("false");
    } else {
        std::cout << "Setting value to true" << std::endl;
        (*currentOption)->setValue("true");
    }
}
