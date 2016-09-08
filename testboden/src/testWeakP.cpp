#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;

class Helper : public Base
{
public:
	Helper(bool* pDeleted = nullptr)
	{
        _pDeleted = pDeleted;
	}

    ~Helper()
    {
        if(_pDeleted!=nullptr)
            *_pDeleted = true;
    }

	void addRef() const override
	{
		_addCounter++;

		Base::addRef();
	}

	void releaseRef() const override
	{
		_releaseCounter++;

		Base::releaseRef();
	}

	void verifyCounters(int expectedAddCounter, int expectedReleaseCounter)
	{
		REQUIRE( _addCounter == expectedAddCounter);
		REQUIRE( _releaseCounter == expectedReleaseCounter);
	}


	mutable int		_addCounter = 0;
	mutable int		_releaseCounter = 0;

    bool* _pDeleted = nullptr;
};


class SubHelper : public Helper
{
public:
    SubHelper(bool* pDeleted = nullptr)
        : Helper(pDeleted)
	{
	}

};


template<class ArgType>
void testConstructToStrongDestruct(Helper& helper, ArgType helperArg)
{   
    // clear the counters
    helper._addCounter = 0;
    helper._releaseCounter = 0;

    SECTION("constructDestruct")
    {
        {
            WeakP<Helper> w(helperArg);

            // should not have added a reference
            helper.verifyCounters(0, 0);
        }

        // should still not have added a reference-
        helper.verifyCounters(0, 0);
    }

    SECTION("constructToStrongDestruct")
    {
        {
            WeakP<Helper> w(helperArg);

            // should not have added a reference
            helper.verifyCounters(0, 0);

            P<Helper> p = w.toStrong();
            REQUIRE( p!=nullptr );
            
            // The strong pointer should have added a ref
            helper.verifyCounters(1, 0);
        }
        
        helper.verifyCounters(1, 1);
    }
}

template<class ArgType>
void testAssignWithPreCreatedWeakP(WeakP<Helper>& w, Helper& helper, ArgType helperArg)
{   
    w = helperArg;

    P<Helper> p = w.toStrong();
    REQUIRE( p.getPtr() == &helper );

}

template<class ArgType>
void testAssign(Helper& helper, ArgType helperArg)
{   
    // clear the counters
    helper._addCounter = 0;
    helper._releaseCounter = 0;

    SECTION("withPrevValue")
    {
        Helper helper2;
        WeakP<Helper> w(&helper2);

        testAssignWithPreCreatedWeakP(w, helper, helperArg);
    }

    SECTION("withoutPrevValue")
    {
        WeakP<Helper> w;

        testAssignWithPreCreatedWeakP(w, helper, helperArg);
    }
}

TEST_CASE("WeakP")
{
	SECTION("constructToStrongDestruct")
	{
        SECTION("cpointer")
        {
            Helper helper;
            testConstructToStrongDestruct(helper, &helper);
        }

        SECTION("subclass cpointer")
        {
            SubHelper helper;
            testConstructToStrongDestruct(helper, &helper);
        }

        SECTION("P")
        {
            Helper helper;

            testConstructToStrongDestruct(helper,  P<Helper>(&helper) );
        }

        SECTION("subclass P")
        {
            SubHelper helper;

            testConstructToStrongDestruct(helper,  P<SubHelper>(&helper) );
        }

        SECTION("WeakP")
        {
            Helper helper;

            WeakP<Helper> w(&helper);

            testConstructToStrongDestruct(helper,  w );
        }

        SECTION("subclass WeakP")
        {
            SubHelper helper;

            WeakP<SubHelper> w(&helper);

            testConstructToStrongDestruct(helper,  w );
        }
    }

    SECTION("assign")
	{
        SECTION("cpointer")
        {
            Helper helper;
            testAssign(helper, &helper);
        }

        SECTION("subclass cpointer")
        {
            SubHelper helper;
            testAssign(helper, &helper);
        }

        SECTION("P")
        {
            Helper helper;

            testAssign(helper,  P<Helper>(&helper) );
        }

        SECTION("subclass P")
        {
            SubHelper helper;

            testAssign(helper,  P<SubHelper>(&helper) );
        }

        SECTION("WeakP")
        {
            Helper helper;

            WeakP<Helper> w(&helper);

            testAssign(helper,  w );
        }

        SECTION("subclass WeakP")
        {
            SubHelper helper;

            WeakP<SubHelper> w(&helper);

            testAssign(helper,  w );
        }
    }

    SECTION("toStrong")
    {
        SECTION("defaultInit")
        {
            WeakP<Helper> w;

            REQUIRE( w.toStrong() == nullptr );
        }

        SECTION("nullInit")
        {
            WeakP<Helper> w(nullptr);

            REQUIRE( w.toStrong() == nullptr );
        }

        SECTION("nullPointerInit")
        {
            P<Helper> p;
            WeakP<Helper> w(p);

            REQUIRE( w.toStrong() == nullptr );
        }

        SECTION("objectOK")
        {
            Helper helper;

            P<Helper> p;

		    {
                WeakP<Helper> w(&helper);

                p = w.toStrong();           
                REQUIRE( p!=nullptr );

                // should have added a reference
                helper.verifyCounters(1, 0);
            }

            // references should still be the same
            helper.verifyCounters(1, 0);

            p = nullptr;

            // now the reference should have been deleted
            helper.verifyCounters(1, 1);
        }

        SECTION("objectGone")
        {
            bool deleted = false;
            P<Helper> p = newObj<Helper>(&deleted);

            WeakP<Helper> w(p);

            // this will delete the object
            p = nullptr;

            REQUIRE( deleted );

            REQUIRE( w.toStrong() == nullptr );
        }
    }


#if BDN_HAVE_THREADS

    SECTION("manyThreadCreateWeakRefs")
    {
        bool        deleted = false;
        P<Helper>   p = newObj<Helper>(&deleted);

        std::list< std::future<void> > futureList;

        for(int i=0; i<100; i++)
        {
            futureList.push_back(
                Thread::exec(
                    [p]
                    {
                        WeakP<Helper> w(&p);

                        w.toStrong();
                    }) );
        }

        // wait for the threads to finish
        for( auto f: futureList)
            f.get();

        p->verifyCounters(100, 100);

        REQUIRE( p->getRefCount() == 1);

        REQUIRE( !deleted );

        p = nullptr;
        REQUIRE( deleted );
    }


    SECTION("manyThreadCreateWeakRefsWhileObjectIsDeleted")
    {
        bool        deleted = false;
        P<Helper>   p = newObj<Helper>(&deleted);

        std::list< std::future<void> > futureList;

        std::atomic<int> successCounter;
        
        WeakP<Helper> w(p);

        for(int i=0; i<100; i++)
        {
            futureList.push_back(
                Thread::exec(
                    [w, &successCounter]
                    {
                        WeakP<Helper> w2(w);

                        if(i==99)
                            Thread::sleepSeconds(1);

                        if(w2.toStrong()!=nullptr)
                            successCounter++;
                    }) );
        }

        REQUIRE( !deleted );

        // do NOT wait for the threads to finish, but immediately release the pointer.
        p = nullptr;

        REQUIRE( deleted );

        // then wait for the threads to finish
        for( auto f: futureList)
            f.get();
        
        // now at least one thread should not have been successful in getting the object
        // (the last one waits a second before it tries, so that one should at least not have gotten it).
        int successCount = successCounter;

        REQUIRE( successCount<=99 );
    }

#endif

}