#ifndef DRICONF3_DRIVERCONFIGURATION_H
#define DRICONF3_DRIVERCONFIGURATION_H

#include "GPUInfo.h"

class DriverConfiguration : public GPUInfo {
private:
    int screen;

public:
    DriverConfiguration();

    const int &getScreen() const;

    void setScreen(int screen);
};

#endif
