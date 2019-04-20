#include "../adriconf/DriverConfiguration.h"
#include "gtest/gtest.h"

class DriverConfigurationTest : public ::testing::Test
{
public:
    std::shared_ptr<DriverConfiguration> testApp;

    DriverConfigurationTest() {
        testApp = std::make_shared<DriverConfiguration>();
    }
};

TEST_F (DriverConfigurationTest, defaultValuesTes) {
    int screenNumber = testApp->getScreen();

    EXPECT_EQ(0, screenNumber);
}

TEST_F (DriverConfigurationTest, screenNumberTest) {
    testApp->setScreen(2);

    EXPECT_EQ(2, testApp->getScreen());

    //duplicate case
    testApp->setScreen(2);
    EXPECT_EQ(2, testApp->getScreen());

    //screen number change case
    testApp->setScreen(1);
    EXPECT_EQ(1, testApp->getScreen());
}
