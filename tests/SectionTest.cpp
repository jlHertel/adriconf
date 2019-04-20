#include "../adriconf/Section.h"
#include "gtest/gtest.h"

class SectionTest : public ::testing::Test
{
public:
    Section *testApp;

    SectionTest() {
        testApp = new Section();
    }
};

TEST_F (SectionTest, defaultsTest) {
    EXPECT_EQ(0, testApp->getOptions().size());
    EXPECT_EQ("", testApp->getDescription());
}

TEST_F (SectionTest, descriptionTest) {
    testApp->setDescription("Awesome driver!");
    EXPECT_EQ("Awesome driver!", testApp->getDescription());

    //duplicate case
    testApp->setDescription("Awesome driver!");
    EXPECT_EQ("Awesome driver!", testApp->getDescription());

    //change case
    testApp->setDescription("Super driver");
    EXPECT_EQ("Super driver", testApp->getDescription());
}

TEST_F (SectionTest, optionTest) {
    DriverOption option0;
    option0.setName("opt0");
    testApp = testApp->addOption(option0);

    EXPECT_EQ("opt0", testApp->getOptions().front().getName());

    DriverOption option1;
    option1.setName("opt1");
    testApp = testApp->addOption(option1);

    EXPECT_EQ("opt0", testApp->getOptions().front().getName());
    EXPECT_EQ("opt1", testApp->getOptions().back().getName());
}

TEST_F (SectionTest, sortDriverOptionsTest) {
    DriverOption option0;
    option0.setType(DriverOptionType::ENUM); //2
    testApp = testApp->addOption(option0);

    DriverOption option1;
    option1.setName("opt1"); //4
    testApp = testApp->addOption(option1);

    DriverOption option2;
    option2.setType(DriverOptionType::BOOL); //1
    testApp = testApp->addOption(option2);

    DriverOption option3;
    option3.setType(DriverOptionType::INT); //3
    testApp = testApp->addOption(option3);

    testApp->sortOptions();

    std::list<DriverOption> options = testApp->getOptions();
    EXPECT_EQ(DriverOptionType::BOOL, options.front().getType());
    options.pop_front();
    EXPECT_EQ(DriverOptionType::ENUM, options.front().getType());
    options.pop_front();
    EXPECT_EQ(DriverOptionType::INT, options.front().getType());
    options.pop_front();
    EXPECT_EQ("opt1", options.front().getName());
    options.pop_front();
}
