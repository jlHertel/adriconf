#include "../adriconf/ConfigurationResolver.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "LoggerMock.h"

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
    LoggerMock logger;
    Glib::ustring warnMsg("User-defined driver 'r600g' on screen '0' doesn't have a driver loaded on system. Configuration removed.");
    EXPECT_CALL(logger, warning(warnMsg))
            .Times(1);

    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setScreen(0);
    d->setDriver("r600g");

    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::removeInvalidDrivers(availableDrivers, userDefinedDevices, &logger);

    EXPECT_EQ(0, userDefinedDevices.size());
}

TEST_F(RemoveInvalidDriversTest, incorrectDriverName) {
    LoggerMock logger;
    Glib::ustring warnMsg("User-defined driver 'i965' on screen '1' doesn't have a driver loaded on system. Configuration removed.");
    EXPECT_CALL(logger, warning(warnMsg)).Times(1);

    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setScreen(1);
    d->setDriver("i965");

    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::removeInvalidDrivers(availableDrivers, userDefinedDevices, &logger);

    EXPECT_EQ(0, userDefinedDevices.size());
}

TEST_F(RemoveInvalidDriversTest, correctScreenAndDriver) {
    LoggerMock logger;

    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setScreen(0);
    d->setDriver("i965");

    Device_ptr d2 = std::make_shared<Device>();
    d2->setScreen(1);
    d2->setDriver("r600g");

    userDefinedDevices.emplace_back(d);
    userDefinedDevices.emplace_back(d2);

    ConfigurationResolver::removeInvalidDrivers(availableDrivers, userDefinedDevices, &logger);

    EXPECT_EQ(2, userDefinedDevices.size());
}


class FilterDriverUnsupportedOptionsTest : public ::testing::Test {
public:
    std::list<DriverConfiguration> availableDrivers;
    std::map<Glib::ustring, GPUInfo_ptr> availableGPUs;

    FilterDriverUnsupportedOptionsTest() {
        DriverConfiguration conf1;
        conf1.setDriverName("i965");
        conf1.setScreen(0);

        Section performanceSection;
        performanceSection.setDescription("Performance");

        DriverOption boReuse;
        boReuse.setName("bo_reuse");
        boReuse.setDefaultValue("0");
        performanceSection.addOption(boReuse);

        DriverOption mesaNoError;
        mesaNoError.setName("mesa_no_error");
        mesaNoError.setDefaultValue("0");
        performanceSection.addOption(mesaNoError);

        std::list<Section> sections;
        sections.emplace_back(performanceSection);
        conf1.setSections(sections);

        availableDrivers.emplace_back(conf1);

        /* Setup GPUs */
        GPUInfo_ptr radeon = std::make_shared<GPUInfo>();
        radeon->setPciId("pci-radeon");
        radeon->setDriverName("r600g");
        radeon->setSections(sections);


        GPUInfo_ptr intel = std::make_shared<GPUInfo>();
        intel->setPciId("pci-intel");
        intel->setDriverName("i965");

        Section imageQuality;
        imageQuality.setDescription("Image Quality");

        DriverOption preciseTrig;
        preciseTrig.setName("precise_trig");
        preciseTrig.setDefaultValue("false");
        imageQuality.addOption(preciseTrig);

        std::list<Section> sectionsIntel;
        sectionsIntel.emplace_back(imageQuality);
        intel->setSections(sectionsIntel);


        availableGPUs["pci-radeon"] = radeon;
        availableGPUs["pci-intel"] = intel;
    }
};

TEST_F(FilterDriverUnsupportedOptionsTest, invalidOption) {
    LoggerMock logger;
    Glib::ustring warnMessage("Driver 'i965' doesn't support option 'invalid_name' on application 'App Name'. Option removed.");
    EXPECT_CALL(logger, warning(warnMessage)).Times(1);

    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setDriver("i965");
    d->setScreen(0);

    Application_ptr app = std::make_shared<Application>();
    app->setName("App Name");
    ApplicationOption_ptr opt = std::make_shared<ApplicationOption>();
    opt->setName("invalid_name");

    app->addOption(opt);
    d->addApplication(app);
    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::filterDriverUnsupportedOptions(availableDrivers, userDefinedDevices, availableGPUs, &logger);

    EXPECT_EQ(0, app->getOptions().size());
}

TEST_F(FilterDriverUnsupportedOptionsTest, validOption) {
    LoggerMock logger;

    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setDriver("i965");
    d->setScreen(0);

    Application_ptr app = std::make_shared<Application>();
    app->setName("App Name");
    ApplicationOption_ptr opt = std::make_shared<ApplicationOption>();
    opt->setName("bo_reuse");

    app->addOption(opt);
    d->addApplication(app);
    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::filterDriverUnsupportedOptions(availableDrivers, userDefinedDevices, availableGPUs, &logger);

    EXPECT_EQ(1, app->getOptions().size());
}

TEST_F(FilterDriverUnsupportedOptionsTest, invalidOptionPrime) {
    LoggerMock logger;
    Glib::ustring warnMessage("Driver 'r600g' doesn't support option 'invalid_name' on application 'App Name'. Option removed.");
    EXPECT_CALL(logger, warning(warnMessage)).Times(1);

    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setDriver("i965");
    d->setScreen(0);

    Application_ptr app = std::make_shared<Application>();
    app->setName("App Name");
    ApplicationOption_ptr opt = std::make_shared<ApplicationOption>();
    opt->setName("invalid_name");
    app->addOption(opt);

    ApplicationOption_ptr optDevice = std::make_shared<ApplicationOption>();
    optDevice->setName("device_id");
    optDevice->setValue("pci-radeon");
    app->addOption(optDevice);


    d->addApplication(app);
    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::filterDriverUnsupportedOptions(availableDrivers, userDefinedDevices, availableGPUs, &logger);

    EXPECT_EQ(1, app->getOptions().size());
}

TEST_F(FilterDriverUnsupportedOptionsTest, validOptionPrime) {
    LoggerMock logger;

    std::list<Device_ptr> userDefinedDevices;
    Device_ptr d = std::make_shared<Device>();
    d->setDriver("i965");
    d->setScreen(0);

    Application_ptr app = std::make_shared<Application>();
    app->setName("App Name");
    ApplicationOption_ptr opt = std::make_shared<ApplicationOption>();
    opt->setName("precise_trig");
    app->addOption(opt);

    ApplicationOption_ptr optDevice = std::make_shared<ApplicationOption>();
    optDevice->setName("device_id");
    optDevice->setValue("pci-intel");
    app->addOption(optDevice);


    d->addApplication(app);
    userDefinedDevices.emplace_back(d);

    ConfigurationResolver::filterDriverUnsupportedOptions(availableDrivers, userDefinedDevices, availableGPUs, &logger);

    EXPECT_EQ(2, app->getOptions().size());
}


class AddMissingApplicationsTest : public ::testing::Test {
public:
    Device_ptr sourceDevice;

    AddMissingApplicationsTest() {
        sourceDevice = std::make_shared<Device>();

        Application_ptr app1 = std::make_shared<Application>();
        app1->setName("App 1");
        app1->setExecutable("app1");

        ApplicationOption_ptr option1 = std::make_shared<ApplicationOption>();
        option1->setName("option_1");
        option1->setValue("value_1");
        app1->addOption(option1);

        ApplicationOption_ptr option2 = std::make_shared<ApplicationOption>();
        option2->setName("option_2");
        option2->setValue("value_2");
        app1->addOption(option2);

        sourceDevice->addApplication(app1);


        Application_ptr app2 = std::make_shared<Application>();
        app2->setName("App 2");
        app2->setExecutable("app2");

        ApplicationOption_ptr option3 = std::make_shared<ApplicationOption>();
        option3->setName("option_3");
        option3->setValue("value_3");
        app2->addOption(option3);

        ApplicationOption_ptr option4 = std::make_shared<ApplicationOption>();
        option4->setName("option_4");
        option4->setValue("value_4");
        app2->addOption(option4);

        sourceDevice->addApplication(app2);
    }
};

TEST_F(AddMissingApplicationsTest, missingApplication) {
    Device_ptr targetDevice = std::make_shared<Device>();

    Application_ptr app1 = std::make_shared<Application>();
    app1->setName("App Test");
    app1->setExecutable("tester");

    ApplicationOption_ptr option1 = std::make_shared<ApplicationOption>();
    option1->setName("option_1");
    option1->setValue("value_1");
    app1->addOption(option1);

    ApplicationOption_ptr option2 = std::make_shared<ApplicationOption>();
    option2->setName("option_2");
    option2->setValue("value_2");
    app1->addOption(option2);

    sourceDevice->addApplication(app1);

    ConfigurationResolver::addMissingApplications(sourceDevice, targetDevice);

    EXPECT_EQ(3, targetDevice->getApplications().size());
}

TEST_F(AddMissingApplicationsTest, existingApplication) {
    Device_ptr targetDevice = std::make_shared<Device>();

    Application_ptr app1 = std::make_shared<Application>();
    app1->setName("App Test");
    app1->setExecutable("app2");

    ApplicationOption_ptr option1 = std::make_shared<ApplicationOption>();
    option1->setName("option_test");
    option1->setValue("value_teste");
    app1->addOption(option1);

    sourceDevice->addApplication(app1);

    ConfigurationResolver::addMissingApplications(sourceDevice, targetDevice);

    EXPECT_EQ(2, targetDevice->getApplications().size());
}