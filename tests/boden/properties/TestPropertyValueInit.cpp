#include <bdn/property/Property.h>
#include <gtest/gtest.h>

using std::string;

namespace dminit
{

    class Person
    {
      public:
        bdn::Property<string> name = string("Albert");
        bdn::Property<int> age = 30;
    };
}

using namespace dminit;

TEST(Old_Property, PropertyValueInit)
{
    Person p;

    EXPECT_EQ(p.name, "Albert");
    EXPECT_EQ(p.age, 30);
}
