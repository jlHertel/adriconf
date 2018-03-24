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


class AddMissingDriverOptionsTest : public ::testing::Test {
public:
    Application_ptr app;

    AddMissingDriverOptionsTest() {
        app = std::make_shared<Application>();

    }
};

/* Checks if the missing options have been added */
TEST_F(AddMissingDriverOptionsTest, missingOptionCount) {
    std::map<Glib::ustring, Glib::ustring> driverOptions;
    driverOptions["bo_reuse"] = "1";
    driverOptions["mesa_no_errors"] = "0";

    ConfigurationResolver::addMissingDriverOptions(app, driverOptions);

    EXPECT_EQ(2, app->getOptions().size());
}

/* Checks if the already existing options maintain their values */
TEST_F(AddMissingDriverOptionsTest, unchangedOptionValue) {
    ApplicationOption_ptr appOpt = std::make_shared<ApplicationOption>();
    appOpt->setName("bo_reuse");
    appOpt->setValue("0");

    app->addOption(appOpt);


    std::map<Glib::ustring, Glib::ustring> driverOptions;
    driverOptions["bo_reuse"] = "1";
    driverOptions["mesa_no_errors"] = "0";

    ConfigurationResolver::addMissingDriverOptions(app, driverOptions);

    std::map<Glib::ustring, Glib::ustring> appOptions = app->getOptionsAsMap();

    EXPECT_EQ("0", appOptions["bo_reuse"]);
    EXPECT_EQ("0", appOptions["mesa_no_errors"]);
}


class RemoveInvalidDriversTest : public ::testing::Test {
public:
    std::list<DriverConfiguration> availableDrivers;

    RemoveInvalidDriversTest() {
        DriverConfiguration conf1, conf2;
        conf1.setDriverName("i965");
        conf1.setScreen(0);

        conf2.setDriverName("r600g");
        conf2.setScreen(1);

        availableDrivers.emplace_back(conf1);
        availableDrivers.emplace_back(conf2);
    }
};

TEST_F(RemoveInvalidDriversTest, incorrectScreenNumber) {
    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setScreen(0);
    d->setDriver("r600g");

    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::removeInvalidDrivers(availableDrivers, userDefinedDevices);

    EXPECT_EQ(0, userDefinedDevices.size());
}

TEST_F(RemoveInvalidDriversTest, incorrectDriverName) {
    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setScreen(1);
    d->setDriver("i965");

    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::removeInvalidDrivers(availableDrivers, userDefinedDevices);

    EXPECT_EQ(0, userDefinedDevices.size());
}

TEST_F(RemoveInvalidDriversTest, correctScreenAndDriver) {
    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setScreen(0);
    d->setDriver("i965");

    Device_ptr d2 = std::make_shared<Device>();
    d2->setScreen(1);
    d2->setDriver("r600g");

    userDefinedDevices.emplace_back(d);
    userDefinedDevices.emplace_back(d2);

    ConfigurationResolver::removeInvalidDrivers(availableDrivers, userDefinedDevices);

    EXPECT_EQ(2, userDefinedDevices.size());
}