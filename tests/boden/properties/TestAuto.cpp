#include "ComplexType.h"
#include <bdn/property/Property.h>
#include <gtest/gtest.h>

using std::string;

namespace auto_keyword
{

    class Person
    {
      public:
        bdn::Property<string> name = string("Albert");
        bdn::Property<int> age = 30;
        bdn::Property<ComplexType> complex = ComplexType{2, "string"};
    };
}

using namespace auto_keyword;

TEST(Old_Property, Auto)
{
    Person p;

    auto name = *p.name;
    auto age = *p.age;
    auto complex = *p.complex;

    EXPECT_EQ(name, "Albert");
    EXPECT_EQ(age, 30);

    EXPECT_EQ(complex.someInt, 2);
    EXPECT_EQ(complex.someString, "string");
}
