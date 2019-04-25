#include <bdn/property/Property.h>
#include <gtest/gtest.h>

namespace shadow
{

    class Base
    {
      public:
        bdn::Property<int> id;
    };

    class Sub : public Base
    {
      public:
        bdn::Property<int> id; // shadows Base::id
    };
}

using namespace shadow;

TEST(Old_Property, Shadow)
{
    auto s = std::make_unique<Sub>();
    Base *b = (Base *)s.get();
    s->id = 1;
    b->id = 2;

    EXPECT_EQ(s->id, 1);
    EXPECT_EQ(b->id, 2);
}
