#ifndef ADRICONF_GUI_H
#define ADRICONF_GUI_H

#include <gtkmm.h>
#include <glibmm/i18n.h>
#include "ValueObject/Device.h"
#include "ValueObject/DriverConfiguration.h"
#include "Utils/ConfigurationLoaderInterface.h"
#include "ValueObject/ComboBoxColumn.h"
#include "Utils/ConfigurationResolverInterface.h"
#include "Utils/WriterInterface.h"

class GUI {
private:
    LoggerInterface *logger;
    ConfigurationLoaderInterface *configurationLoader;
    ConfigurationResolverInterface *resolver;
    WriterInterface *writer;

    /* GUI-Related */
    Gtk::Window *pWindow;
    Gtk::AboutDialog aboutDialog;
    Gtk::MenuItem *pMenuAddApplication;
    Gtk::MenuItem *pMenuRemoveApplication;
    ComboBoxColumn comboColumns;

    /* State-related */
    std::list<Device_ptr> systemWideConfiguration;
    std::list<DriverConfiguration> driverConfiguration;
    std::list<Device_ptr> userDefinedConfiguration;
    std::map<Glib::ustring, GPUInfo_ptr> availableGPUs;
    bool isPrimeSetup;
    Application_ptr currentApp;
    DriverConfiguration *currentDriver;
    std::map<Glib::ustring, Gtk::ComboBox *> currentComboBoxes;
    std::map<Glib::ustring, Gtk::SpinButton *> currentSpinButtons;

    /* Helpers */
    Glib::RefPtr<Gtk::Builder> gladeBuilder;
    Glib::ustring locale;

    void setupLocale();

    void drawApplicationSelectionMenu();

    void drawApplicationOptions();

    void setupAboutDialog();

public:
    GUI(LoggerInterface *logger, ConfigurationLoaderInterface *configurationLoader, ConfigurationResolverInterface *resolver, WriterInterface *writer);

    virtual ~GUI();

    Gtk::Window *getWindowPointer();

    /* Signal Handlers */
    void onQuitPressed();

    void onSavePressed();

    void onApplicationSelected(const Glib::ustring &, const Glib::ustring &);

    void onCheckboxChanged(Glib::ustring);

    void onFakeCheckBoxChanged(Glib::ustring);

    void onComboboxChanged(Glib::ustring);

    void onNumberEntryChanged(Glib::ustring);

    void onRemoveApplicationPressed();

    void onAddApplicationPressed();
};

#endif
