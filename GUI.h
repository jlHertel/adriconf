#ifndef ADRICONF_GUI_H
#define ADRICONF_GUI_H

#include <gtkmm.h>
#include <glibmm/i18n.h>
#include "Device.h"
#include "DriverConfiguration.h"
#include "ConfigurationLoader.h"

namespace DRI {
    class GUI {
    private:
        /* GUI-Related */
        Gtk::Window *pWindow;

        /* State-related */
        DRI::Device systemWideConfiguration;
        std::list<DRI::DriverConfiguration> driverConfiguration;
        std::list<DRI::Device> userDefinedConfiguration;

        /* Helpers */
        DRI::ConfigurationLoader configurationLoader;
        Glib::RefPtr<Gtk::Builder> gladeBuilder;
        Glib::ustring locale;

        void setupLocale();
    public:
        GUI();

        virtual ~GUI();

        Gtk::Window *getWindowPointer();

        /* Signal Handlers */
        void onQuitPressed();

        void onSavePressed();
    };
}

#endif
