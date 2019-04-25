#include <bdn/property/Property.h>
#include <gtest/gtest.h>

namespace mi
{

    class A
    {
      public:
        bdn::Property<int> id;
        bdn::Property<int> onlyInA;
    };

    class B
    {
      public:
        bdn::Property<int> id;
        bdn::Property<int> onlyInB;
    };

    class C : public A, public B
    {
      public:
        bdn::Property<int> id = {B::id.backing()};  // C::id disambiguates to B::id
        bdn::Property<int> aid = {A::id.backing()}; // could use aid to access A::id without casting
    };
}

using namespace mi;

TEST(Old_Property, MultipleInheritance)
{
    C *c = new C();
    B *b = (B *)c;
    A *a = (A *)c;

    a->id = 1;
    b->id = 2;
    c->id = 3;

    c->onlyInA = 4;
    c->onlyInB = 5;

    EXPECT_EQ(a->id, 1);
    EXPECT_EQ(b->id, 3);
    EXPECT_EQ(c->id, 3);
    EXPECT_EQ(c->aid, 1);

    EXPECT_EQ(c->onlyInA, 4);
    EXPECT_EQ(c->onlyInB, 5);
    EXPECT_EQ(a->onlyInA, 4);
    EXPECT_EQ(b->onlyInB, 5);

    delete c;
}
