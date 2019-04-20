#include "../adriconf/DriverOption.h"
#include "gtest/gtest.h"

class DriverOptionTest : public ::testing::Test
{
public:
    DriverOption* testApp;
    DriverOption* testApp1;

    DriverOptionTest() {
        testApp = new DriverOption();
        testApp1 = new DriverOption();

        testApp1->setValidValues("01:007");
    }
};

TEST_F (DriverOptionTest, defaultsTest) {
    std::list<std::pair<Glib::ustring, Glib::ustring>> enumValues = testApp->getEnumValues();

    EXPECT_EQ("", testApp->getName());
    EXPECT_EQ("", testApp->getDescription());
    EXPECT_EQ("", testApp->getDefaultValue());
    EXPECT_EQ("", testApp->getValidValues());
    EXPECT_EQ(-1, testApp->getValidValueStart());
    EXPECT_EQ(10000, testApp->getValidValueEnd());
    EXPECT_EQ(4, testApp->getSortValue());
    EXPECT_TRUE(enumValues.empty());
}

TEST_F (DriverOptionTest, nameTest) {
    testApp = testApp->setName("i965");
    EXPECT_EQ("i965", testApp->getName());

    //duplicate case
    testApp = testApp->setName("i965");
    EXPECT_EQ("i965", testApp->getName());

    //name change case
    testApp = testApp->setName("nouveau");
    EXPECT_EQ("nouveau", testApp->getName());
}

TEST_F (DriverOptionTest, descriptionTest) {
    testApp = testApp->setDescription("description0");
    EXPECT_EQ("description0", testApp->getDescription());

    //duplicate case
    testApp = testApp->setDescription("description0");
    EXPECT_EQ("description0", testApp->getDescription());

    //description change case
    testApp = testApp->setDescription("description1");
    EXPECT_EQ("description1", testApp->getDescription());
}

TEST_F (DriverOptionTest, typeTest) {
    testApp = testApp->setType(DriverOptionType::BOOL);
    EXPECT_EQ(DriverOptionType::BOOL, testApp->getType());

    //duplicate case
    testApp = testApp->setType(DriverOptionType::BOOL);
    EXPECT_EQ(DriverOptionType::BOOL, testApp->getType());

    //type change case
    testApp = testApp->setType(DriverOptionType::INT);
    EXPECT_EQ(DriverOptionType::INT, testApp->getType());
}

TEST_F (DriverOptionTest, defaultValueTest) {
    testApp = testApp->setDefaultValue("0:0");
    EXPECT_EQ("0:0", testApp->getDefaultValue());

    //duplicate case
    testApp = testApp->setDefaultValue("0:0");
    EXPECT_EQ("0:0", testApp->getDefaultValue());

    //change case
    testApp = testApp->setDefaultValue("1:1");
    EXPECT_EQ("1:1", testApp->getDefaultValue());
}

TEST_F (DriverOptionTest, validValuesTest) {
    testApp = testApp->setValidValues("vv0:0");
    EXPECT_EQ("vv0:0", testApp->getValidValues());

    //duplicate case
    testApp = testApp->setValidValues("vv0:0");
    EXPECT_EQ("vv0:0", testApp->getValidValues());

    //change case
    testApp = testApp->setValidValues("vv1:1");
    EXPECT_EQ("vv1:1", testApp->getValidValues());
}

TEST_F (DriverOptionTest, enumValuesTest) {
    testApp = testApp->addEnumValue("desc0", "val0");
    ASSERT_TRUE(testApp->getEnumValues().size() == 1);

    std::pair<Glib::ustring, Glib::ustring> enumVal ("desc0", "val0");
    EXPECT_TRUE(testApp->getEnumValues().front() == enumVal);

    //different enumValues case
    testApp = testApp->addEnumValue("desc1", "val1");
    ASSERT_TRUE(testApp->getEnumValues().size() == 2);

    std::pair<Glib::ustring, Glib::ustring> enumVal1 ("desc1", "val1");
    EXPECT_TRUE(testApp->getEnumValues().front() == enumVal);
    EXPECT_TRUE(testApp->getEnumValues().back() == enumVal1);
}

TEST_F (DriverOptionTest, validValueStartTest) {
    EXPECT_EQ(1, testApp1->getValidValueStart());

    //empty start-value case
    testApp1->setValidValues(":007");
    EXPECT_EQ(-1, testApp1->getValidValueStart());

    //both empty case
    testApp1->setValidValues(":");
    EXPECT_EQ(-1, testApp1->getValidValueStart());

    //empty end-value case
    testApp1->setValidValues("00000010:");
    EXPECT_EQ(10, testApp1->getValidValueStart());
}

TEST_F (DriverOptionTest, validValueEndTest) {
    EXPECT_EQ(7, testApp1->getValidValueEnd());

    //empty start-value case
    testApp1->setValidValues(":00999");
    EXPECT_EQ(999, testApp1->getValidValueEnd());

    //both empty case
    testApp1->setValidValues(":");
    EXPECT_EQ(10000, testApp1->getValidValueEnd());

    //empty end-value case
    testApp1->setValidValues("00000010:");
    EXPECT_EQ(10000, testApp1->getValidValueEnd());
}

TEST_F (DriverOptionTest, sortValueTest) {
    testApp->setType(DriverOptionType::BOOL);
    EXPECT_EQ(1, testApp->getSortValue());

    //duplicate case
    testApp->setType(DriverOptionType::BOOL);
    EXPECT_EQ(1, testApp->getSortValue());

    testApp->setType(DriverOptionType::ENUM);
    EXPECT_EQ(2, testApp->getSortValue());

    testApp->setType(DriverOptionType::INT);
    EXPECT_EQ(3, testApp->getSortValue());
}

TEST_F (DriverOptionTest, updateFakeBoolTest) {
    testApp->setType(DriverOptionType::ENUM);
    testApp->setValidValues("0:1");
    testApp->updateFakeBool();
    EXPECT_EQ(DriverOptionType::FAKE_BOOL, testApp->getType());

    testApp->setType(DriverOptionType::INT);
    testApp->setValidValues("12:99");
    testApp->addEnumValue("d0", "v0");
    testApp->updateFakeBool();
    EXPECT_NE(DriverOptionType::FAKE_BOOL, testApp->getType());
}
