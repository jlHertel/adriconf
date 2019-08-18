#include "../adriconf/ValueObject/Application.h"
#include "gtest/gtest.h"

class ApplicationTest : public ::testing::Test
{
public:
    Application_ptr testApp;

    ApplicationTest() {
        testApp = std::make_shared<Application>();
    }
};

TEST_F (ApplicationTest, defaultValuesTest) {
    Glib::ustring name = testApp->getName();
    Glib::ustring executable = testApp->getExecutable();
    std::list<ApplicationOption_ptr> options = testApp->getOptions();
    std::map<Glib::ustring, Glib::ustring> optionsMap = testApp->getOptionsAsMap();
    bool prime = testApp->getIsUsingPrime();
    Glib::ustring primeName = testApp->getPrimeDriverName();
    Glib::ustring pciId = testApp->getDevicePCIId();


    EXPECT_EQ("", name);
    EXPECT_EQ("", executable);
    EXPECT_EQ(0, options.size());
    EXPECT_EQ(0, optionsMap.size());
    EXPECT_FALSE(prime);
    EXPECT_EQ("", primeName);
    EXPECT_EQ("", pciId);
}

TEST_F (ApplicationTest, nameTest) {
    testApp->setName("Watch Dogs");
    Glib::ustring name = testApp->getName();

    EXPECT_EQ("Watch Dogs", name);

    //duplicate case
    testApp->setName("Watch Dogs");
    Glib::ustring name1 = testApp->getName();

    EXPECT_EQ("Watch Dogs", name1);

    //name change case
    testApp->setName("FIFA 2018");
    Glib::ustring name2 = testApp->getName();
    EXPECT_EQ("FIFA 2018", name2);
}

TEST_F (ApplicationTest, executableTest) {
    testApp->setExecutable("Watch Dogs executable");
    Glib::ustring executable = testApp->getExecutable();

    EXPECT_EQ("Watch Dogs executable", executable);

    //duplicate case
    testApp->setExecutable("Watch Dogs executable");
    Glib::ustring executable1 = testApp->getExecutable();

    EXPECT_EQ("Watch Dogs executable", executable1);

    //Executable change case
    testApp->setExecutable("FIFA 2018 executable");
    Glib::ustring executable2 = testApp->getExecutable();
    EXPECT_EQ("FIFA 2018 executable", executable2);
}

TEST_F (ApplicationTest, optionsTest) {
    ApplicationOption_ptr optPtr = std::make_shared<ApplicationOption>();
    optPtr->setName("screen-width");
    testApp->addOption(optPtr);

    ApplicationOption_ptr optPtr1 = std::make_shared<ApplicationOption>();
    optPtr1->setName("refresh-rate");
    testApp->addOption(optPtr1);

    std::list<ApplicationOption_ptr> options = testApp->getOptions();
    ASSERT_EQ(2, options.size());
    EXPECT_EQ("refresh-rate", options.back()->getName());
    EXPECT_EQ("screen-width", options.front()->getName());
}

TEST_F (ApplicationTest, optionsAsMapTest) {
    std::list<ApplicationOption_ptr> options;
    ApplicationOption_ptr optPtr = std::make_shared<ApplicationOption>();
    optPtr->setName("fancy-option");

    ApplicationOption_ptr optPtr1 = std::make_shared<ApplicationOption>();
    optPtr1->setName("locale-option");

    options.emplace_back(optPtr);
    options.emplace_back(optPtr1);

    testApp->setOptions(options);

    std::map<Glib::ustring, Glib::ustring> optMap = testApp->getOptionsAsMap();

    ASSERT_EQ(2, options.size());

    EXPECT_TRUE(optMap.find("fancy-option") != optMap.end());
    EXPECT_TRUE(optMap.find("locale-option") != optMap.end());
}

TEST_F (ApplicationTest, primeTest) {
    testApp->setIsUsingPrime(true);
    EXPECT_TRUE(testApp->getIsUsingPrime());

    testApp->setIsUsingPrime(false);
    EXPECT_FALSE(testApp->getIsUsingPrime());
}

TEST_F (ApplicationTest, primeDiverNameTest) {
    testApp->setPrimeDriverName("i965");
    Glib::ustring name = testApp->getPrimeDriverName();
    EXPECT_EQ("i965", name);

    //duplicate case
    testApp->setPrimeDriverName("i965");
    Glib::ustring name1 = testApp->getPrimeDriverName();
    EXPECT_EQ("i965", name1);

    testApp->setPrimeDriverName("nouveau");
    Glib::ustring name2 = testApp->getPrimeDriverName();
    EXPECT_EQ("nouveau", name2);
}

TEST_F (ApplicationTest, pciIdTest) {
    testApp->setDevicePCIId("pci-test-001");
    Glib::ustring pciId = testApp->getDevicePCIId();
    EXPECT_EQ("pci-test-001", pciId);

    //duplicate case
    testApp->setDevicePCIId("pci-test-001");
    Glib::ustring pciId1 = testApp->getDevicePCIId();
    EXPECT_EQ("pci-test-001", pciId1);

    testApp->setDevicePCIId("pci-test-changed");
    Glib::ustring pciId2 = testApp->getDevicePCIId();
    EXPECT_EQ("pci-test-changed", pciId2);
}
