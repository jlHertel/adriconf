#include "GUI.h"

#include <boost/locale.hpp>
#include "Parser.h"
#include "ConfigurationResolver.h"
#include "DRIQuery.h"
#include "Writer.h"

#include <iostream>
#include <fstream>

DRI::GUI::GUI() {
    this->setupLocale();

    /* Load the configurations */
    this->driverConfiguration = this->configurationLoader.loadDriverSpecificConfiguration(this->locale);
    this->systemWideConfiguration = this->configurationLoader.loadSystemWideConfiguration();
    this->userDefinedConfiguration = this->configurationLoader.loadUserDefinedConfiguration();

    /* Merge all the options in a complete structure */
    this->userDefinedConfiguration = DRI::ConfigurationResolver::mergeOptionsForDisplay(
            this->systemWideConfiguration, this->driverConfiguration, this->userDefinedConfiguration
    );

    /* Filter invalid options */
    this->userDefinedConfiguration = DRI::ConfigurationResolver::filterDriverUnsupportedOptions(
            this->driverConfiguration, this->userDefinedConfiguration
    );

    /* Load the GUI file */
    this->gladeBuilder = Gtk::Builder::create();
    this->gladeBuilder->add_from_file("DriConf.glade");

    /* Extract the main object */
    this->gladeBuilder->get_widget("mainwindow", this->pWindow);

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
    this->drawApplicationSelectionMenu();
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

        /* Sort the applications again to maintain a good human GUI */
        for (auto &driver : this->userDefinedConfiguration) {
            driver.sortApplications();
        }

        Gtk::RadioButtonGroup appRadioGroup;
        bool groupInitialized = false;

        for (auto &driver : this->userDefinedConfiguration) {

            if (this->currentSelectedDriver.empty()) {
                this->currentSelectedDriver = driver.getDriver();
            }

            Gtk::MenuItem *driverMenuItem = Gtk::manage(new Gtk::MenuItem);
            driverMenuItem->set_visible(true);
            driverMenuItem->set_label(driver.getDriver());

            Gtk::Menu *driverSubMenu = Gtk::manage(new Gtk::Menu);
            driverSubMenu->set_visible(true);
            for (auto &possibleApp : driver.getApplications()) {
                Gtk::RadioMenuItem *appMenuItem = Gtk::manage(new Gtk::RadioMenuItem);
                appMenuItem->set_visible(true);
                appMenuItem->set_label(possibleApp.getName());

                if (!groupInitialized) {
                    appRadioGroup = appMenuItem->get_group();
                    groupInitialized = true;
                } else {
                    appMenuItem->set_group(appRadioGroup);
                }

                if (this->currentSelectedDriver == driver.getDriver() && possibleApp.getExecutable().empty()) {
                    appMenuItem->set_active(true);
                }

                appMenuItem->signal_toggled().connect(sigc::bind<Glib::ustring, Glib::ustring>(
                        sigc::mem_fun(this, &DRI::GUI::onApplicationSelected),
                        driver.getDriver(), possibleApp.getExecutable()
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

    /* TODO: Redraw the screen items */

    std::cout << "Selected driver \"" << driverName << "\" and \"" << applicationName << "\" executable." << std::endl;

}
