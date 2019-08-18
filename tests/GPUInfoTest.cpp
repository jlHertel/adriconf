#include "../adriconf/ValueObject/GPUInfo.h"
#include "gtest/gtest.h"

class GPUInfoTest : public ::testing::Test
{
public:
    GPUInfo_ptr testApp;

    GPUInfoTest() {
        testApp = std::make_shared<GPUInfo>();
    }
};

TEST_F (GPUInfoTest, defaultsTest) {
    EXPECT_EQ("", testApp->getPciId());
    EXPECT_EQ("", testApp->getDriverName());
    EXPECT_EQ("", testApp->getDeviceName());
    EXPECT_EQ("", testApp->getVendorName());
    EXPECT_EQ(0, testApp->getVendorId());
    EXPECT_EQ(0, testApp->getDeviceId());
    EXPECT_TRUE(testApp->getSections().empty());
    EXPECT_TRUE(testApp->getOptionsMap().empty());
}

TEST_F (GPUInfoTest, pciIdTest) {
    testApp->setPciId("00101");
    EXPECT_EQ("00101", testApp->getPciId());

    //duplicate case
    testApp->setPciId("00101");
    EXPECT_EQ("00101", testApp->getPciId());

    //change case
    testApp->setPciId("00561651");
    EXPECT_EQ("00561651", testApp->getPciId());
}

TEST_F (GPUInfoTest, driverNameTest) {
    testApp->setDriverName("i965");
    EXPECT_EQ("i965", testApp->getDriverName());

    //duplicate case
    testApp->setDriverName("i965");
    EXPECT_EQ("i965", testApp->getDriverName());

    //change case
    testApp->setDriverName("nouveau");
    EXPECT_EQ("nouveau", testApp->getDriverName());
}

TEST_F (GPUInfoTest, deviceNameTest) {
    testApp->setDeviceName("device0");
    EXPECT_EQ("device0", testApp->getDeviceName());

    //duplicate case
    testApp->setDeviceName("device0");
    EXPECT_EQ("device0", testApp->getDeviceName());

    //change case
    testApp->setDeviceName("device1");
    EXPECT_EQ("device1", testApp->getDeviceName());
}

TEST_F (GPUInfoTest, VendorNameTest) {
    testApp->setVendorName("intel");
    EXPECT_EQ("intel", testApp->getVendorName());

    //duplicate case
    testApp->setVendorName("intel");
    EXPECT_EQ("intel", testApp->getVendorName());

    //change case
    testApp->setVendorName("AMD");
    EXPECT_EQ("AMD", testApp->getVendorName());
}

TEST_F (GPUInfoTest, VendorIdTest) {
    testApp->setVendorId(01456);
    EXPECT_EQ(01456, testApp->getVendorId());

    //duplicate case
    testApp->setVendorId(01456);
    EXPECT_EQ(01456, testApp->getVendorId());

    //change case
    testApp->setVendorId(000653);
    EXPECT_EQ(000653, testApp->getVendorId());
}

TEST_F (GPUInfoTest, deviceIdTest) {
    testApp->setDeviceId(01456);
    EXPECT_EQ(01456, testApp->getDeviceId());

    //duplicate case
    testApp->setDeviceId(01456);
    EXPECT_EQ(01456, testApp->getDeviceId());

    //change case
    testApp->setDeviceId(000653);
    EXPECT_EQ(000653, testApp->getDeviceId());
}

TEST_F (GPUInfoTest, equalsOperatorTest) {
    GPUInfo_ptr testApp1 = std::make_shared<GPUInfo>();
    testApp->setDeviceId(0456);
    testApp->setVendorId(1567);

    testApp1->setDeviceId(8998);
    testApp1->setVendorId(2313);

    //both different case
    EXPECT_FALSE(testApp == testApp1);

    //only vendorId different case
    testApp1->setDeviceId(0456);
    EXPECT_FALSE(testApp == testApp1);

    //only deviceId different case
    testApp1->setDeviceId(8998);
    testApp1->setVendorId(1567);
    EXPECT_FALSE(testApp == testApp1);

    //both same case
    testApp1->setDeviceId(0456);
    testApp1->setVendorId(1567);
    EXPECT_FALSE(testApp == testApp1);
}

TEST_F (GPUInfoTest, sectionTest) {
    std::list<Section> sections0;
    Section s;
    s.setDescription("bla bla..");
    sections0.emplace_back(s);

    //(process:30123): GLib-CRITICAL **: 11:47:06.629: g_utf8_collate: assertion 'str1 != NULL' failed
    //might be due to the Section* which will be returned in L132
    //WARNNING after adding below line..
    EXPECT_EQ("bla bla..", testApp->getSections().front().getDescription());
}

TEST_F (GPUInfoTest, driverOptionsMapTest) {
    std::map<Glib::ustring, Glib::ustring> optionMap;
    std::list<Section> sections0;
    Section s;
    DriverOption drOpt;
    drOpt.setName("name0");
    drOpt.setDefaultValue("007");
    s.addOption(drOpt);
    sections0.emplace_back(s);

    testApp->setSections(sections0);
    optionMap["name0"] = "007";

    EXPECT_EQ(optionMap, testApp->getOptionsMap());
}
