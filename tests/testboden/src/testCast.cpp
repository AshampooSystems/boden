#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/cast.h>

using namespace bdn;

class A : public Base
{
};

class SubA : public A
{
};

class SubSubA : public SubA
{
};

class B
{
};

void verifyConst(void *p, bool constExpected) { REQUIRE(!constExpected); }

void verifyConst(const void *p, bool constExpected) { REQUIRE(constExpected); }

template <class InPtr> void testTryCast(InPtr subA, bool constExpected)
{
    REQUIRE(tryCast<SubA>(subA) == subA);
    REQUIRE(tryCast<A>(subA) == subA);
    REQUIRE(tryCast<B>(subA) == nullptr);
    REQUIRE(tryCast<SubSubA>(subA) == nullptr);

    verifyConst(tryCast<SubA>(subA), constExpected);
    verifyConst(tryCast<A>(subA), constExpected);
    verifyConst(tryCast<B>(subA), constExpected);
    verifyConst(tryCast<SubSubA>(subA), constExpected);
}

TEST_CASE("tryCast")
{
    SECTION("null")
    {
        A *a = nullptr;

        REQUIRE(tryCast<A>(a) == nullptr);
        REQUIRE(tryCast<B>(a) == nullptr);
        REQUIRE(tryCast<SubA>(a) == nullptr);
    }

    SECTION("non-null")
    {
        SubA subA;

        SECTION("non-const")
        testTryCast<SubA *>(&subA, false);

        SECTION("const")
        testTryCast<const SubA *>(&subA, true);

        SECTION("P")
        {
            P<SubA> subAPointer = &subA;

            testTryCast<P<SubA>>(subAPointer, false);
        }

        SECTION("PC")
        {
            P<const SubA> subAPointer = &subA;

            testTryCast<P<const SubA>>(subAPointer, true);
        }
    }
}

template <class InPtr> void testCast(InPtr subA, bool constExpected)
{

    REQUIRE(cast<SubA>(subA) == subA);
    REQUIRE(cast<A>(subA) == subA);
    REQUIRE_THROWS_AS(cast<B>(subA), CastError);
    REQUIRE_THROWS_AS(cast<SubSubA>(subA), CastError);

    verifyConst(cast<SubA>(subA), constExpected);
    verifyConst(cast<A>(subA), constExpected);
}

TEST_CASE("cast")
{
    SECTION("null")
    {
        A *a = nullptr;

        REQUIRE(cast<A>(a) == nullptr);
        REQUIRE(cast<B>(a) == nullptr);
        REQUIRE(cast<SubA>(a) == nullptr);
    }

    SECTION("non-null")
    {
        SubA subA;

        SECTION("non-const")
        testCast<SubA *>(&subA, false);

        SECTION("const")
        testCast<const SubA *>(&subA, true);

        SECTION("P")
        {
            P<SubA> subAPointer = &subA;

            testCast<P<SubA>>(subAPointer, false);
        }

        SECTION("PC")
        {
            P<const SubA> subAPointer = &subA;

            testCast<P<const SubA>>(subAPointer, true);
        }
    }
}
