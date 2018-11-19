#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;

TEST_CASE("newObj")
{
    SECTION("refCount")
    {
        P<Base> p = newObj<Base>();

        REQUIRE(p->getRefCount() == 1);
    }

    SECTION("instantDelete")
    {
        class InstantDeleteHelper : public Base
        {
          public:
            InstantDeleteHelper(bool *deleted) { _deleted = deleted; }

            ~InstantDeleteHelper() { *_deleted = true; }

          protected:
            bool *_deleted;
        };

        bool deleted = false;

        newObj<InstantDeleteHelper>(&deleted);

        REQUIRE(deleted);
    }
}
