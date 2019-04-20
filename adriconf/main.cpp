#include <iostream>
#include <gtkmm.h>
#include "GUI.h"
#include "DRIQuery.h"
#include <gtkmm/messagedialog.h>

int main(int argc, char *argv[]) {
    try {
        /* Start the GUI work */
        auto app = Gtk::Application::create(argc, argv, "br.com.jeanhertel.adriconf");

        char *protocol = std::getenv("XDG_SESSION_TYPE");

        DRIQuery check;
        if (!check.isSystemSupported()) {
            return 1;
        }
        if (std::string(protocol) == "wayland") {
            std::cout << _("adriconf running on Wayland\n");
        } else if (std::string(protocol) == "x11"){
            std::cout << _("adriconf running on X11\n");

            //Error window pops up even when a screen has a driver which we can't configure
            if (!check.canHandle()) {
                std::cerr << _("Not all screens have open source drivers") << std::endl;

                //pop up error window here
                Gtk::MessageDialog *errorDialog = new Gtk::MessageDialog("Closed source driver(s) detected!",
                                                                        false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
                errorDialog->set_secondary_text("Currently adriconf cannot handle closed source drivers.");
                if (errorDialog->run()) {
                    errorDialog->close();
                    return 0;
                }
            }
        } else {
            std::cout << _("Unknow display server protocol being used\n");
            return 1;
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
    catch (const std::exception &ex) {
        std::cerr << "Generic Error: " << ex.what() << std::endl;
    }

    return 0;
}
