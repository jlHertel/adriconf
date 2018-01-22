#include <iostream>
#include <gtkmm.h>
#include <glibmm/i18n.h>
#include "GUI.h"

int main(int argc, char *argv[]) {
    /* Start the GUI work */
    auto app = Gtk::Application::create(argc, argv, "br.com.jeanhertel.adriconf");
    try {
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