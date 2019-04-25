#include "ComplexType.h"
#include "Person.h"
#include <bdn/property/Property.h>
#include <gtest/gtest.h>

using std::string;

namespace copy
{
    class Ref
    {
      public:
        int id = 1;
        int &idRef = id;
    };

    class InternallyBacked
    {
      public:
        bdn::Property<int> height = 20;
        bdn::Property<int> width = 30;
    };
}

using namespace copy;

TEST(Old_Property, CppRefRunningIntoBadAccess)
{
    Ref *r1 = new Ref();
    Ref *r2 = new Ref(*r1);
    delete r1;
    // int id = r2->idRef; // Bad access
    delete r2;
}

TEST(Old_Property, CopyInternallyBackedProperty)
{
    // Copying an internally-backed Property is permitted and results in the expected behavior,
    // i.e. two independent internally-backed properties with the same value:
    bdn::Property<int> number = 10;
    bdn::Property<int> numberCopy;
    numberCopy = number;

    EXPECT_EQ(number, 10);
    EXPECT_EQ(numberCopy, 10);

    EXPECT_EQ(number, numberCopy);
}

TEST(Old_Property, CopyInternallyBackedObject)
{
    InternallyBacked i1{10, 20};
    InternallyBacked i2;
    // Copying an object which has internally-backed Property data members only is permitted and
    // will work as expected:
    i2 = i1;

    EXPECT_EQ(i1.height, i2.height);
    EXPECT_EQ(i1.width, i2.width);
}

TEST(Old_Property, CopyNonInternallyBackedProperty)
{
    bdn::Property<int> number = 10;

    // Copying externally backed Properties is not permitted as their lifecycle is not defined
    // from the perspective of the Property class. E.g. the following won't compile and errors
    // with "Call to implicitly-deleted copy constructor of 'Property<int, ValueBacking<int>>'.
    // bdn::Property<int, ValueBacking<int>> numberCopy = number;

    // However, if someone just wanted to copy the current value of number into another Property
    // instance, that can be done by casting:
    bdn::Property<int> numberCopy = (int)number;

    EXPECT_EQ(numberCopy, number);
}

class DefaultConstructThenCopy : public testing::Test
{
  protected:
    void SetUp() override
    {
        jacqueline.id = 1;

        EXPECT_EQ(jacqueline.name, "Jacqueline");
        EXPECT_EQ(jacqueline.age, 15);
        EXPECT_EQ(jacqueline.complex->someInt, 42);
        EXPECT_EQ(jacqueline.complex->someString, "running out of ideas");

        jack.id = 2;
        jack = jacqueline;
        jack.id = 2;

        EXPECT_EQ(jack.name, "Jacqueline");
        EXPECT_EQ(jack.age, 15);
        EXPECT_EQ(jack.complex->someInt, 42);
        EXPECT_EQ(jack.complex->someString, "running out of ideas");
    }

    Person jacqueline;
    Person jack;
};

TEST_F(DefaultConstructThenCopy, ValueBacking)
{
    jack.name = "Jack";
    jack.age = 17;
    jack.complex = ComplexType{43, "test"};

    string jname = jacqueline.name;

    EXPECT_EQ(jacqueline.name, "Jacqueline");
    EXPECT_EQ(jacqueline.age, 15);
    EXPECT_EQ(jack.name, "Jack");
    EXPECT_EQ(jack.age, 17);

    EXPECT_EQ(jacqueline.complex->someInt, 42);
    EXPECT_EQ(jacqueline.complex->someString, "running out of ideas");
    EXPECT_EQ(jack.complex->someInt, 43);
    EXPECT_EQ(jack.complex->someString, "test");
}

TEST_F(DefaultConstructThenCopy, GetterSetterMemberBacking)
{
    EXPECT_EQ(jacqueline.height, 155);
    EXPECT_EQ(jacqueline._height, 155);
    EXPECT_EQ(jack.height, 155);
    EXPECT_EQ(jack._height, 155);

    jack.height = 170;

    EXPECT_EQ(jacqueline.height, 155);
    EXPECT_EQ(jacqueline._height, 155);
    EXPECT_EQ(jack.height, 170);
    EXPECT_EQ(jack._height, 170);
}

TEST_F(DefaultConstructThenCopy, TransformMemberBacking)
{
    jack.height = 170;

    EXPECT_EQ(jacqueline.heightInInches, 155 * 2.54);
    EXPECT_EQ(jack.heightInInches, 170 * 2.54);

    EXPECT_TRUE(jack.heightInInches.backing() != jacqueline.heightInInches.backing());
}
