#include "DriverConfiguration.h"

const int &DriverConfiguration::getScreen() const {
    return screen;
}

void DriverConfiguration::setScreen(int screen) {
    DriverConfiguration::screen = screen;
}

DriverConfiguration::DriverConfiguration() : screen(0) {}
