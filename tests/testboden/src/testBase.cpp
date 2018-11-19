#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;

TEST_CASE("Base")
{
    Base base;

    SECTION("refcount")
    {
        REQUIRE(base.getRefCount() == 1);

        base.addRef();

        REQUIRE(base.getRefCount() == 2);

        base.releaseRef();

        REQUIRE(base.getRefCount() == 1);
    }

    SECTION("deleted when refcount=0")
    {
        class TestBase : public Base
        {
          public:
            TestBase(bool *deleted) { _deleted = deleted; }

            ~TestBase() { *_deleted = true; }

          protected:
            bool *_deleted;
        };

        bool deleted = false;

        TestBase *test = new (Base::RawNew::Use) TestBase(&deleted);

        REQUIRE(!deleted);

        test->releaseRef();

        REQUIRE(deleted);
    }

    SECTION("isBeingDeletedBecauseReferenceCountReachedZero")
    {
        class TestBase : public Base
        {
          public:
            TestBase(bool *deleted) { _deleted = deleted; }

            ~TestBase() { *_deleted = isBeingDeletedBecauseReferenceCountReachedZero(); }

          protected:
            bool *_deleted;
        };

        bool deleted = false;

        TestBase *test = new (Base::RawNew::Use) TestBase(&deleted);

        REQUIRE(!deleted);

        test->releaseRef();

        REQUIRE(deleted);
    }

    SECTION("revive during deletion")
    {
        class TestBase : public Base
        {
          public:
            TestBase(int *deleteThisCounter) { _deleteThisCounter = deleteThisCounter; }

          protected:
            void deleteThis() override
            {
                (*_deleteThisCounter)++;

                reviveDuringDeleteThis().detachPtr();
            }

            int *_deleteThisCounter;
        };

        int deleteThisCounter = 0;

        TestBase *test = new (Base::RawNew::Use) TestBase(&deleteThisCounter);

        REQUIRE(deleteThisCounter == 0);

        test->releaseRef();

        REQUIRE(deleteThisCounter == 1);

        REQUIRE(test->getRefCount() == 1);

        REQUIRE(!test->isBeingDeletedBecauseReferenceCountReachedZero());

        delete test;
    }

    SECTION("addRef/releaseRef in destructor")
    {
        // we only test here that the additional releaseRef does not
        // cause a second deletion (which would cause a crash).
        class TestBase : public Base
        {
          public:
            ~TestBase()
            {
                addRef();
                releaseRef();
            }
        };

        TestBase *test = new (Base::RawNew::Use) TestBase;

        test->releaseRef();
    }
}