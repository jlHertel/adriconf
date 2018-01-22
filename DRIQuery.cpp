#include <giomm.h>
#include "DRIQuery.h"

DRIQuery::DRIQuery() {
    this->getScreenDriver = (glXGetScreenDriver_t *) glXGetProcAddress((const GLubyte *) "glXGetScreenDriver");
    this->getDriverConfig = (glXGetDriverConfig_t *) glXGetProcAddress((const GLubyte *) "glXGetDriverConfig");

    if (!this->getScreenDriver || !this->getDriverConfig) {
        std::cerr << "Error getting function pointers. LibGL must be too old." << std::endl;
    }
}

std::list<DriverConfiguration> DRIQuery::queryDriverConfigurationOptions(const Glib::ustring &locale) {
    std::list<DriverConfiguration> configurations;

    Display *display;

    if (!(display = XOpenDisplay(nullptr))) {
        std::cerr << "Couldn't open X display" << std::endl;
        return configurations;
    }

    /* Call glXGetClientString to load vendor libs on glvnd enabled systems */
    glXGetClientString(display, GLX_EXTENSIONS);

    int screenCount = ScreenCount (display);

    for (int i = 0; i < screenCount; i++) {
        DriverConfiguration config;
        config.setScreen(i);

        auto driverName = (*(this->getScreenDriver))(display, i);
        config.setDriver(driverName);

        auto driverOptions = (*(this->getDriverConfig))(driverName);
        Glib::ustring options(driverOptions);

        auto parsedSections = Parser::parseAvailableConfiguration(options, locale);
        config.setSections(parsedSections);

        configurations.emplace_back(config);
    }

    return std::move(configurations);
}

