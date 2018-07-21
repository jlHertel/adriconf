#include "../DriverOption.h"
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
    Glib::ustring name = testApp->getName();
    Glib::ustring description = testApp->getDescription();
    Glib::ustring dValue = testApp->getDefaultValue();
    Glib::ustring validValue = testApp->getValidValues();
    int validValueStart = testApp->getValidValueStart();//-1
    int validValueEnd = testApp->getValidValueEnd();    //10000
    int getSortValues = testApp->getSortValue();        //4
    bool isFake = testApp->isFakeBool();                //false
    std::list<std::pair<Glib::ustring, Glib::ustring>> enumValues = testApp->getEnumValues();

    EXPECT_EQ("", name);
    EXPECT_EQ("", description);
    EXPECT_EQ("", dValue);
    EXPECT_EQ("", validValue);
    EXPECT_EQ(-1, validValueStart);
    EXPECT_EQ(10000, validValueEnd);
    EXPECT_EQ(4, getSortValues);
    EXPECT_EQ(false, isFake);
    EXPECT_TRUE(enumValues.empty());
}

TEST_F (DriverOptionTest, nameTest) {
    testApp = testApp->setName("i965");
    Glib::ustring name = testApp->getName();

    EXPECT_EQ("i965", name);

    //duplicate case
    testApp = testApp->setName("i965");
    Glib::ustring name1 = testApp->getName();

    EXPECT_EQ("i965", name1);

    //name change case
    testApp = testApp->setName("nouveau");
    Glib::ustring name2 = testApp->getName();
    EXPECT_EQ("nouveau", name2);
}

TEST_F (DriverOptionTest, descriptionTest) {
    testApp = testApp->setDescription("description0");
    Glib::ustring desc0 = testApp->getDescription();

    EXPECT_EQ("description0", desc0);

    //duplicate case
    testApp = testApp->setDescription("description0");
    Glib::ustring desc1 = testApp->getDescription();

    EXPECT_EQ("description0", desc1);

    //description change case
    testApp = testApp->setDescription("description1");
    Glib::ustring desc2 = testApp->getDescription();
    EXPECT_EQ("description1", desc2);
}

TEST_F (DriverOptionTest, typeTest) {
    testApp = testApp->setType("type0");
    Glib::ustring type0 = testApp->getType();

    EXPECT_EQ("type0", type0);

    //duplicate case
    testApp = testApp->setType("type0");
    Glib::ustring type1 = testApp->getType();

    EXPECT_EQ("type0", type1);

    //type change case
    testApp = testApp->setType("type1");
    Glib::ustring type2 = testApp->getType();
    EXPECT_EQ("type1", type2);
}

TEST_F (DriverOptionTest, defaultValueTest) {
    testApp = testApp->setDefaultValue("0:0");
    Glib::ustring dv0 = testApp->getDefaultValue();

    EXPECT_EQ("0:0", dv0);

    //duplicate case
    testApp = testApp->setDefaultValue("0:0");
    Glib::ustring dv1 = testApp->getDefaultValue();

    EXPECT_EQ("0:0", dv1);

    //change case
    testApp = testApp->setDefaultValue("1:1");
    Glib::ustring dv2 = testApp->getDefaultValue();
    EXPECT_EQ("1:1", dv2);
}

TEST_F (DriverOptionTest, validValuesTest) {
    testApp = testApp->setValidValues("vv0:0");
    Glib::ustring vv0 = testApp->getValidValues();

    EXPECT_EQ("vv0:0", vv0);

    //duplicate case
    testApp = testApp->setValidValues("vv0:0");
    Glib::ustring vv1 = testApp->getValidValues();

    EXPECT_EQ("vv0:0", vv1);

    //change case
    testApp = testApp->setValidValues("vv1:1");
    Glib::ustring vv2 = testApp->getValidValues();
    EXPECT_EQ("vv1:1", vv2);
}

TEST_F (DriverOptionTest, enumValuesTest) {
    testApp = testApp->addEnumValue("desc0", "val0");

    ASSERT_TRUE(testApp->getEnumValues().size() == 1);
    std::pair<Glib::ustring, Glib::ustring> enumVal ("desc0", "val0");
    EXPECT_TRUE(testApp->getEnumValues().front() == enumVal);

    //duplicate case ???
    //check addEnumValue method in driverOption class.
    //It dosen't check whether the list already contains enumValue


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
    testApp->setType("bool");
    EXPECT_EQ(1, testApp->getSortValue());

    //duplicate case
    testApp->setType("bool");
    EXPECT_EQ(1, testApp->getSortValue());

    testApp->setType("enum");
    EXPECT_EQ(2, testApp->getSortValue());

    testApp->setType("int");
    EXPECT_EQ(3, testApp->getSortValue());
}

TEST_F (DriverOptionTest, isFakeTest) {
    testApp->setType("enum");
    testApp->setValidValues("0:1");
    EXPECT_TRUE(testApp->isFakeBool());

    testApp->setType("int");
    testApp->setValidValues("12:99");
    testApp->addEnumValue("d0", "v0");
    EXPECT_FALSE(testApp->isFakeBool());
}
