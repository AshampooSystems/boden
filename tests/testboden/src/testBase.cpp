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
            TestBase(bool *pDeleted) { _pDeleted = pDeleted; }

            ~TestBase() { *_pDeleted = true; }

          protected:
            bool *_pDeleted;
        };

        bool deleted = false;

        TestBase *pTest = new (Base::RawNew::Use) TestBase(&deleted);

        REQUIRE(!deleted);

        pTest->releaseRef();

        REQUIRE(deleted);
    }

    SECTION("isBeingDeletedBecauseReferenceCountReachedZero")
    {
        class TestBase : public Base
        {
          public:
            TestBase(bool *pDeleted) { _pDeleted = pDeleted; }

            ~TestBase() { *_pDeleted = isBeingDeletedBecauseReferenceCountReachedZero(); }

          protected:
            bool *_pDeleted;
        };

        bool deleted = false;

        TestBase *pTest = new (Base::RawNew::Use) TestBase(&deleted);

        REQUIRE(!deleted);

        pTest->releaseRef();

        REQUIRE(deleted);
    }

    SECTION("revive during deletion")
    {
        class TestBase : public Base
        {
          public:
            TestBase(int *pDeleteThisCounter) { _pDeleteThisCounter = pDeleteThisCounter; }

          protected:
            void deleteThis() override
            {
                (*_pDeleteThisCounter)++;

                reviveDuringDeleteThis().detachPtr();
            }

            int *_pDeleteThisCounter;
        };

        int deleteThisCounter = 0;

        TestBase *pTest = new (Base::RawNew::Use) TestBase(&deleteThisCounter);

        REQUIRE(deleteThisCounter == 0);

        pTest->releaseRef();

        REQUIRE(deleteThisCounter == 1);

        REQUIRE(pTest->getRefCount() == 1);

        REQUIRE(!pTest->isBeingDeletedBecauseReferenceCountReachedZero());

        delete pTest;
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

        TestBase *pTest = new (Base::RawNew::Use) TestBase;

        pTest->releaseRef();
    }
}