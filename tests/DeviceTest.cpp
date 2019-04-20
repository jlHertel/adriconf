#include "../adriconf/Device.h"
#include "gtest/gtest.h"

class DeviceTest : public ::testing::Test {
public :
    Device_ptr d;

    DeviceTest() {
        d = std::make_shared<Device>();
    }
};

TEST_F (DeviceTest, defaultValues) {
    Glib::ustring driverName = d->getDriver();
    const int screen = d->getScreen();
    std::list<Application_ptr> appList = d->getApplications();

    EXPECT_EQ("", driverName);
    EXPECT_EQ(-1, screen);
    EXPECT_EQ(0, appList.size());
}

TEST_F (DeviceTest, driver) {
    d->setDriver("i915");

    EXPECT_EQ("i915", d->getDriver());
}

TEST_F (DeviceTest, driverChange) {
    d->setDriver("i915");
    d->setDriver("i915-new");

    EXPECT_EQ("i915-new", d->getDriver());

    //duplicate driver case
    d->setDriver("i915-new");
    EXPECT_EQ("i915-new", d->getDriver());
}

TEST_F (DeviceTest, screen) {
    d->setScreen(5);

    EXPECT_EQ(5, d->getScreen());
}

TEST_F (DeviceTest, screenChange) {
    d->setScreen(2);
    d->setScreen(3);

    EXPECT_EQ(3, d->getScreen());

    //duplicate screen case
    d->setScreen(3);
    EXPECT_EQ(3, d->getScreen());
}

class DeviceApplicationTest : public ::testing::Test {
public:
    Device_ptr d;

    DeviceApplicationTest() {
        d = std::make_shared<Device>();
        Application_ptr testApp1 = std::make_shared<Application>();
        testApp1->setName("app1");
        testApp1->setExecutable("app1-executable");
        d->addApplication(testApp1);
    }
};

TEST_F (DeviceApplicationTest, addApplicationTest) {
    //if this is not true then there is no need to test below cases
    ASSERT_EQ(1, d->getApplications().size());

    EXPECT_EQ("app1", d->getApplications().front()->getName());
    EXPECT_EQ("app1-executable", d->getApplications().front()->getExecutable());

    Application_ptr testApp2 = std::make_shared<Application>();
    testApp2->setName("app2");
    testApp2->setExecutable("app2-executable");
    d->addApplication(testApp2);

    ASSERT_EQ(2, d->getApplications().size());

    EXPECT_EQ("app2", d->getApplications().back()->getName());
    EXPECT_EQ("app2-executable", d->getApplications().back()->getExecutable());
}

TEST_F (DeviceApplicationTest, findApplicationTest) {
    ASSERT_EQ(1, d->getApplications().size());

    EXPECT_EQ("app1-executable", d->findApplication("app1-executable")->getExecutable());
    EXPECT_EQ("app1", d->findApplication("app1-executable")->getName());
}

/*
 * First add app1(added by constructor), app3, app2 to Device_ptr then check
 * the order by poping the values from back of Application_ptr list.
 *
 * The sorted order should be app1, app2, app3
 * */
TEST_F (DeviceApplicationTest, sortApplicationTest) {
    ASSERT_EQ(1, d->getApplications().size());

    Application_ptr testApp2 = std::make_shared<Application>();
    testApp2->setName("app3");
    testApp2->setExecutable("app3-executable");
    d->addApplication(testApp2);

    Application_ptr testApp3 = std::make_shared<Application>();
    testApp3->setName("app2");
    testApp3->setExecutable("app2-executable");
    d->addApplication(testApp3);

    d->sortApplications();

    std::list<Application_ptr> appList = d->getApplications();

    EXPECT_EQ("app3", appList.back()->getName());
    appList.pop_back();

    EXPECT_EQ("app2", appList.back()->getName());
    appList.pop_back();

    EXPECT_EQ("app1", appList.back()->getName());
}
