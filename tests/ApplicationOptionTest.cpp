#include "../adriconf/ApplicationOption.h"
#include "gtest/gtest.h"

class ApplicationOptionTest : public ::testing::Test
{
public:
    ApplicationOption_ptr testApp;

    ApplicationOptionTest() {
        testApp = std::make_shared<ApplicationOption>();
    }
};

TEST_F (ApplicationOptionTest, defaultValuesTest) {
    Glib::ustring name = testApp->getName();
    Glib::ustring value = testApp->getValue();

    EXPECT_EQ("", name);
    EXPECT_EQ("", value);
}

TEST_F (ApplicationOptionTest, nameTest) {
    testApp->setName("tcl_mode");

    EXPECT_EQ("tcl_mode", testApp->getName());

    //duplicate case
    testApp->setName("tcl_mode");
    EXPECT_EQ("tcl_mode", testApp->getName());

    //name change case
    testApp->setName("vblank-mode");
    EXPECT_EQ("vblank-mode", testApp->getName());
}

TEST_F (ApplicationOptionTest, valueTest) {
    testApp->setValue("0");

    EXPECT_EQ("0", testApp->getValue());

    //duplicate case
    testApp->setValue("0");
    EXPECT_EQ("0", testApp->getValue());

    //value change case
    testApp->setValue("3");
    EXPECT_EQ("3", testApp->getValue());
}
