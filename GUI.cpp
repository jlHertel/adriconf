#include "GUI.h"

#include <boost/locale.hpp>
#include "Section.h"
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
    Gtk::Menu *pApplicationMenu;
    this->gladeBuilder->get_widget("ApplicationMenu", pApplicationMenu);

    /*
     * TODO: Rewrite this in a better maintanable code
     *
     * if (pApplicationMenu) {
        Gtk::RadioButtonGroup AppRadioGroup;
        bool groupInitialized = false;

        Gtk::RadioButtonGroup appRadioGroup;
        auto firstDriver = userDevices.begin();
        for (auto &driverApp : firstDriver->getApplications()) {
            Gtk::RadioMenuItem *appRadioItem = Gtk::manage(new Gtk::RadioMenuItem());
            appRadioItem->set_label(driverApp.getName());
            appRadioItem->set_visible(true);
            appRadioItem->signal_activate().connect([]() {
                std::cout << "App changed" << std::endl;
            });

            if (!groupInitialized) {
                appRadioGroup = appRadioItem->get_group();
                appRadioItem->set_active(true);
                groupInitialized = true;
            } else {
                appRadioItem->set_group(appRadioGroup);
            }

            pApplicationMenu->add(*appRadioItem);
        }
    }*/
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
