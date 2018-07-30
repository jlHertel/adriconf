#include <iostream>
#include <gtkmm.h>
#include "GUI.h"
#include "DRIQuery.h"

int main(int argc, char *argv[]) {
    /* Start the GUI work */
    auto app = Gtk::Application::create(argc, argv, "br.com.jeanhertel.adriconf");
    try {
        DRIQuery check;

        //Error window pops up even when a screen has a driver which we can't configure
        if (!check.canHandle()) {
            std::cerr << "Not all screens have open source drivers" << std::endl;
            //pop up error window here
            return 0;
        }

        GUI gui;

        /* No need to worry about the window pointer as the gui object owns it */
        Gtk::Window *pWindow = gui.getWindowPointer();

        app->run(*pWindow);
    }
    catch (const Glib::FileError &ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Glib::MarkupError &ex) {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Gtk::BuilderError &ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
