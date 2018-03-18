#include "../ConfigurationResolver.h"
#include "gtest/gtest.h"

class UpdatePrimeApplicationsTest : public ::testing::Test {
public:
    std::map<Glib::ustring, GPUInfo_ptr> gpus;
    std::list<Device_ptr> devices;
    Application_ptr app;
    ApplicationOption_ptr opt;

    UpdatePrimeApplicationsTest() {
        GPUInfo_ptr gpu = std::make_shared<GPUInfo>();
        gpu->setDriverName("i965");
        gpu->setPciId("pci-testing-10");
        gpus["pci-testing-10"] = gpu;

        Device_ptr device = std::make_shared<Device>();
        app = std::make_shared<Application>();
        opt = std::make_shared<ApplicationOption>();
        opt->setName("device_id");


        app->addOption(opt);
        device->addApplication(app);
        devices.emplace_back(device);
    }
};

TEST_F(UpdatePrimeApplicationsTest, InvalidGPU) {
    opt->setValue("pci-testing-invalid");

    ConfigurationResolver::updatePrimeApplications(devices, gpus);

    EXPECT_FALSE(app->getIsUsingPrime());
}

TEST_F(UpdatePrimeApplicationsTest, CorrectGPU) {
    opt->setValue("pci-testing-10");

    ConfigurationResolver::updatePrimeApplications(devices, gpus);

    EXPECT_TRUE(app->getIsUsingPrime());
    EXPECT_EQ("pci-testing-10", app->getDevicePCIId());
    EXPECT_EQ("i965", app->getPrimeDriverName());
}