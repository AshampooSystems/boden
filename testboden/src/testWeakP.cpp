#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;

class WeakPHelper : public Base
{
public:
	WeakPHelper(bool* pDeleted = nullptr)
    : _addCounter(0)
    , _releaseCounter(0)
	{
        _pDeleted = pDeleted;
	}

    ~WeakPHelper()
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


	mutable std::atomic<int>		_addCounter;
	mutable std::atomic<int>		_releaseCounter;

    bool* _pDeleted = nullptr;
};


class SubWeakPHelper : public WeakPHelper
{
public:
    SubWeakPHelper(bool* pDeleted = nullptr)
        : WeakPHelper(pDeleted)
	{
	}

};


template<class ArgType>
void testConstructToStrongDestruct(WeakPHelper& helper, ArgType helperArg)
{   
    // clear the counters
    helper._addCounter = 0;
    helper._releaseCounter = 0;


    SECTION("constructDestruct")
    {
        {
            WeakP<WeakPHelper> w(helperArg);

            // should not have added a reference
            helper.verifyCounters(0, 0);
        }

        // should still not have added a reference-
        helper.verifyCounters(0, 0);
    }

    SECTION("constructToStrongDestruct")
    {
        {
            WeakP<WeakPHelper> w(helperArg);

            // should not have added a reference
            helper.verifyCounters(0, 0);

            P<WeakPHelper> p = w.toStrong();
            REQUIRE( p!=nullptr );
            REQUIRE( p.getPtr() == &helper );
            
            // The strong pointer should have added a ref twice and released one
            helper.verifyCounters(2, 1);
        }
        
        helper.verifyCounters(2, 2);
    }
}

template<class ArgType>
void testAssignWithPreCreatedWeakP(WeakP<WeakPHelper>& w, WeakPHelper& helper, ArgType helperArg)
{   
    w = helperArg;

    P<WeakPHelper> p = w.toStrong();
    REQUIRE( p.getPtr() == &helper );

}

template<class ArgType>
void testAssign(WeakPHelper& helper, ArgType helperArg)
{   
    // clear the counters
    helper._addCounter = 0;
    helper._releaseCounter = 0;

    SECTION("withPrevValue")
    {
        WeakPHelper helper2;
        WeakP<WeakPHelper> w(&helper2);

        testAssignWithPreCreatedWeakP(w, helper, helperArg);
    }

    SECTION("withoutPrevValue")
    {
        WeakP<WeakPHelper> w;

        testAssignWithPreCreatedWeakP(w, helper, helperArg);
    }
}


TEST_CASE("WeakP")
{
	SECTION("constructToStrongDestruct")
	{
        SECTION("cpointer")
        {
            WeakPHelper helper;

            testConstructToStrongDestruct(helper, &helper);
        }

        SECTION("subclass cpointer")
        {
            SubWeakPHelper helper;
            testConstructToStrongDestruct(helper, &helper);
        }

        SECTION("P")
        {
            WeakPHelper helper;

            testConstructToStrongDestruct(helper,  P<WeakPHelper>(&helper) );
        }

        SECTION("subclass P")
        {
            SubWeakPHelper helper;

            testConstructToStrongDestruct(helper,  P<SubWeakPHelper>(&helper) );
        }

        SECTION("WeakP")
        {
            WeakPHelper helper;

            WeakP<WeakPHelper> w(&helper);

            testConstructToStrongDestruct(helper,  w );
        }

        SECTION("subclass WeakP")
        {
            SubWeakPHelper helper;

            WeakP<SubWeakPHelper> w(&helper);

            testConstructToStrongDestruct(helper,  w );
        }
    }

    SECTION("assign")
	{
        SECTION("cpointer")
        {
            WeakPHelper helper;
            testAssign(helper, &helper);
        }

        SECTION("subclass cpointer")
        {
            SubWeakPHelper helper;
            testAssign(helper, &helper);
        }

        SECTION("P")
        {
            WeakPHelper helper;

            testAssign(helper,  P<WeakPHelper>(&helper) );
        }

        SECTION("subclass P")
        {
            SubWeakPHelper helper;

            testAssign(helper,  P<SubWeakPHelper>(&helper) );
        }

        SECTION("WeakP")
        {
            WeakPHelper helper;

            WeakP<WeakPHelper> w(&helper);

            testAssign(helper,  w );
        }

        SECTION("subclass WeakP")
        {
            SubWeakPHelper helper;

            WeakP<SubWeakPHelper> w(&helper);

            testAssign(helper,  w );
        }
    }

    SECTION("toStrong")
    {
        SECTION("defaultInit")
        {
            WeakP<WeakPHelper> w;

            REQUIRE( w.toStrong() == nullptr );
        }

        SECTION("nullInit")
        {
            WeakP<WeakPHelper> w(nullptr);

            REQUIRE( w.toStrong() == nullptr );
        }

        SECTION("nullPointerInit")
        {
            P<WeakPHelper> p;
            WeakP<WeakPHelper> w(p);

            REQUIRE( w.toStrong() == nullptr );
        }

        SECTION("objectOK")
        {
            WeakPHelper helper;

            P<WeakPHelper> p;

		    {
                WeakP<WeakPHelper> w(&helper);

                p = w.toStrong();           
                REQUIRE( p!=nullptr );

                // should have added two references and released one
                helper.verifyCounters(2, 1);
            }

            // references should still be the same
            helper.verifyCounters(2, 1);

            p = nullptr;

            // now the reference should have been deleted
            helper.verifyCounters(2, 2);
        }

        SECTION("objectGone")
        {
            bool deleted = false;
            P<WeakPHelper> p = newObj<WeakPHelper>(&deleted);

            WeakP<WeakPHelper> w(p);

            // this will delete the object
            p = nullptr;

            REQUIRE( deleted );

            REQUIRE( w.toStrong() == nullptr );
        }
    }


#if BDN_HAVE_THREADS

    SECTION("manyThreadCreateWeakRefs")
    {
        bool            deleted = false;
        P<WeakPHelper>  p = newObj<WeakPHelper>(&deleted);
        
        // use a c pointer to pass to the lambda, so that no addrefs and releases are caused
        // by the ,an
        WeakPHelper*    pCPointer = p;  

        std::list< std::future<void> > futureList;

        p->_addCounter = 0;
        p->_releaseCounter = 0;

        for(int i=0; i<100; i++)
        {
            futureList.push_back(
                Thread::exec(
                    [pCPointer]
                    {
                        WeakP<WeakPHelper> w(pCPointer);

                        w.toStrong();
                    }) );
        }

        // wait for the threads to finish
        for( auto& f: futureList)
            f.get();

        p->verifyCounters(200, 200);

        REQUIRE( p->getRefCount() == 1);

        REQUIRE( !deleted );

        p = nullptr;
        REQUIRE( deleted );
    }


    SECTION("manyThreadCreateWeakRefsWhileObjectIsDeleted")
    {
        bool        deleted = false;
        P<WeakPHelper>   p = newObj<WeakPHelper>(&deleted);

        std::list< std::future<void> > futureList;

        std::atomic<int> successCounter(0);
        
        WeakP<WeakPHelper> w(p);

        for(int i=0; i<100; i++)
        {
            futureList.push_back(
                Thread::exec(
                    [w, &successCounter, i]
                    {
                        WeakP<WeakPHelper> w2(w);

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
        for( auto& f: futureList)
            f.get();
        
        // now at least one thread should not have been successful in getting the object
        // (the last one waits a second before it tries, so that one should at least not have gotten it).
        int successCount = successCounter;

        REQUIRE( successCount<=99 );
    }

#endif

}