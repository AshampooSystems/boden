#include <bdn/property/Property.h>
#include <gtest/gtest.h>

class Base
{
  public:
    bdn::Property<int> id;
};

class Sub : public Base
{
  public:
    bdn::Property<int> id = {Base::id.backing()};
};

TEST(Old_Property, SubclassOverloading)
{
    auto s = std::make_unique<Sub>();
    Base *b = (Base *)s.get();
    b->id = 1;
    s->id = 2;

    EXPECT_EQ(s->id, 2);
    EXPECT_EQ(b->id, 2);

    s.reset();
}
