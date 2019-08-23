#include <iostream>
#include <gtkmm.h>
#include <memory>
#include <gtkmm/messagedialog.h>

#include "GUI.h"
#include "Utils/DRIQuery.h"
#include "Utils/ConfigurationLoader.h"
#include "Logging/Logger.h"
#include "Logging/LoggerInterface.h"


int main(int argc, char *argv[]) {
    char *verbosity = std::getenv("VERBOSITY");
    Logger *logger = new Logger();
    logger->setLevel(LoggerLevel::INFO);

    if (verbosity != nullptr) {
        Glib::ustring verbosityStr(verbosity);
        if (verbosityStr == "DEBUG") {
            logger->setLevel(LoggerLevel::DEBUG);
        }

        if (verbosityStr == "INFO") {
            logger->setLevel(LoggerLevel::INFO);
        }

        if (verbosityStr == "WARNING") {
            logger->setLevel(LoggerLevel::WARNING);
        }

        if (verbosityStr == "ERROR") {
            logger->setLevel(LoggerLevel::ERROR);
        }
    }

    try {
        logger->debug(_("Creating the GTK application object"));

        /* Start the GUI work */
        auto app = Gtk::Application::create(argc, argv, "br.com.jeanhertel.adriconf");

        char *waylandDisplay = std::getenv("WAYLAND_DISPLAY");
        bool isWayland = waylandDisplay != nullptr;

        logger->debug(_("Checking if the system is supported"));
        Parser parser(logger);
        DRIQuery check(logger, &parser, isWayland);
        if (!check.isSystemSupported()) {
            return 1;
        }
        if (isWayland) {
            logger->info(_("adriconf running on Wayland"));
        } else {
            logger->info(_("adriconf running on X11"));

            //Error window pops up even when a screen has a driver which we can't configure
            if (!check.canHandle()) {
                logger->error(_("Not all screens have open source drivers"));

                //pop up error window here
                Gtk::MessageDialog *errorDialog = new Gtk::MessageDialog("Closed source driver(s) detected!",
                                                                         false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
                errorDialog->set_secondary_text("Currently adriconf cannot handle closed source drivers.");
                if (errorDialog->run()) {
                    errorDialog->close();
                    return 0;
                }
            }
        }

        ConfigurationLoader loader(check, logger, &parser);
        GUI gui(logger, &loader);

        /* No need to worry about the window pointer as the gui object owns it */
        Gtk::Window *pWindow = gui.getWindowPointer();

        return app->run(*pWindow);
    }
    catch (const Glib::FileError &ex) {
        logger->error(Glib::ustring::compose("FileError: %1", ex.what()));
        return 1;
    }
    catch (const Glib::MarkupError &ex) {
        logger->error(Glib::ustring::compose("MarkupError: %1", ex.what()));
        return 1;
    }
    catch (const Gtk::BuilderError &ex) {
        logger->error(Glib::ustring::compose("BuilderError: %1", ex.what()));
        return 1;
    }
    catch (const std::exception &ex) {
        logger->error(Glib::ustring::compose("Generic Error: %1", ex.what()));
        return 1;
    }
}
