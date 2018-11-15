#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/PreferredViewSizeManager.h>

using namespace bdn;

static void verifyGet(PreferredViewSizeManager &man, const Size &availableSpace, const Size &expectedResultSize)
{
    Size resultSize;
    bool succeeded = man.get(availableSpace, resultSize);

    REQUIRE(resultSize == expectedResultSize);
    REQUIRE(succeeded == (expectedResultSize != Size::none()));
}

static void verifyGetFails(PreferredViewSizeManager &man, const Size &availableSpace)
{
    verifyGet(man, availableSpace, Size::none());
}

TEST_CASE("PreferredViewSizeManager")
{
    PreferredViewSizeManager man;
    Size result;

    SECTION("empty")
    {
        SECTION("zero space")
        verifyGetFails(man, Size());

        SECTION("infinite space")
        verifyGetFails(man, Size::none());

        SECTION("nonzero space")
        verifyGetFails(man, Size(10, 20));
    }

    SECTION("not empty")
    {
        man.set(Size(10, 100), Size(1, 2));
        man.set(Size(20, 50), Size(3, 4));
        man.set(Size::none(), Size(5000, 6000));

        SECTION("no match - zero")
        verifyGetFails(man, Size());

        SECTION("no match - nonzero")
        verifyGetFails(man, Size(30, 40));

        SECTION("width matches first")
        verifyGetFails(man, Size(10, 40));

        SECTION("width matches second")
        verifyGetFails(man, Size(20, 40));

        SECTION("width matches third")
        verifyGetFails(man, Size(Size::componentNone(), 40));

        SECTION("height matches first")
        verifyGetFails(man, Size(40, 100));

        SECTION("height matches second")
        verifyGetFails(man, Size(40, 50));

        SECTION("height matches third")
        verifyGetFails(man, Size(40, Size::componentNone()));

        SECTION("match first")
        verifyGet(man, Size(10, 100), Size(1, 2));

        SECTION("match second")
        verifyGet(man, Size(20, 50), Size(3, 4));

        SECTION("match third (infinity)")
        verifyGet(man, Size::none(), Size(5000, 6000));

        // if the available space is >= the preferred size for infinite space
        // then that should be returned.
        SECTION("available space = infinite preferred size")
        verifyGet(man, Size(5000, 6000), Size(5000, 6000));

        SECTION("available space > infinite preferred size")
        verifyGet(man, Size(5100, 6100), Size(5000, 6000));

        SECTION("available width infinite, height > infinite preferred size")
        verifyGet(man, Size(Size::componentNone(), 6100), Size(5000, 6000));

        SECTION("available width > infinite preferred size, height > infinite ")
        verifyGet(man, Size(5100, Size::componentNone()), Size(5000, 6000));

        SECTION("available width > infinite preferred size, height smaller")
        verifyGetFails(man, Size(5100, 5999));

        SECTION("available height > infinite preferred size, width smaller")
        verifyGetFails(man, Size(4999, 6100));

        SECTION("available width infinite, height smaller")
        verifyGetFails(man, Size(Size::componentNone(), 5999));

        SECTION("available height infinite, width smaller")
        verifyGetFails(man, Size(4999, Size::componentNone()));
    }
}
