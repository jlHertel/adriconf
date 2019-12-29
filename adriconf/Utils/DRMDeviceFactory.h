#ifndef ADRICONF_DRMDEVICEFACTORY_H
#define ADRICONF_DRMDEVICEFACTORY_H

#include "DRMDeviceFactoryInterface.h"
#include "../ValueObject/DRMDeviceInterface.h"

#include <memory>
#include <vector>

class DRMDeviceFactory : public DRMDeviceFactoryInterface {
public:
    std::vector<std::shared_ptr<DRMDeviceInterface>> getDevices() override;
};


#endif //ADRICONF_DRMDEVICEFACTORY_H
