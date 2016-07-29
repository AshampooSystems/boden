#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/ThreadLocalStorage.h>

#include <bdn/Thread.h>

using namespace bdn;
using namespace bdn::win32;

class TestData
{
public:
	TestData(int v = 17)
	{
		val = v;
	}
	
	int val;
};


class TestData2 : public Base
{
public:
	static int& getConstructed()
	{
		static int constructed=0;

		return constructed;
	}
		

	static int& getDestructed()
	{
		static int destructed=0;

		return destructed;
	}

	TestData2(int v = 17)
	{
		val = v;
		getConstructed()++;
	}

	~TestData2()
	{
		getDestructed()++;
	}

	int val;
};

static ThreadLocalStorage< P<TestData2> > pGlobalThreadLocal;
	
TEST_CASE("win32::ThreadLocalStorage")
{
	

    SECTION( "initial value" )
    {
        SECTION("withConstructor")
        {
            ThreadLocalStorage<TestData> data;

			TestData& ref = data;
    
            REQUIRE( ref.val == 17 );
        }
        
        SECTION("int")
        {
			ThreadLocalStorage<int> data;

            REQUIRE( data == 0 );
        }
        
        SECTION("bool")
        {
			ThreadLocalStorage<bool> data;

            REQUIRE( data == false );
        }
        
        SECTION("void*")
        {
			ThreadLocalStorage<void*> data;
            
            REQUIRE( data==nullptr );
        }
    }
    
    SECTION( "setGet" )
    {
		ThreadLocalStorage<TestData> data;

        REQUIRE( ((TestData&)data).val == 17 );

		data = TestData(19);

		REQUIRE( ((TestData&)data).val == 19 );		        
    }
    
#if BDN_HAVE_THREADS

    SECTION("setGetReleaseOtherThread")
    {
		int constructedBefore = TestData2::getConstructed();
        int destructedBefore = TestData2::getDestructed();        
        
        auto threadResult = Thread::exec(   []()
                        {
							REQUIRE( pGlobalThreadLocal == nullptr );

                            P<TestData2> pData = newObj<TestData2>( 143 );
                            pGlobalThreadLocal = pData;
                            
                            Thread::sleepMillis(3000);
                            
                            // should still have the same pointer
                            REQUIRE( pGlobalThreadLocal==pData );
                        } );
        
        Thread::sleepMillis(1000);

        // a new instance should have been created (but not yet destroyed)
        REQUIRE( TestData2::getConstructed() == constructedBefore+1);
        REQUIRE( TestData2::getDestructed() == destructedBefore);
        
        // in this thread the pointer should still be null for this thread
        REQUIRE( pGlobalThreadLocal==nullptr );
        
        P<TestData2> pData = newObj<TestData2>(42);
        pGlobalThreadLocal = pData;
        
        REQUIRE( pGlobalThreadLocal!=nullptr );
        
        // one more object should exist now
		REQUIRE( TestData2::getConstructed() == constructedBefore+2);
        REQUIRE( TestData2::getDestructed() == destructedBefore);
        
        // wait for the thread to end
        threadResult.get();
        
        // wait a little bit to make sure that the thread cleanup had time to run
        Thread::sleepMillis(2000);
        
        // now the object from the other thread should have been released
        REQUIRE( TestData2::getConstructed() == constructedBefore+2);
        REQUIRE( TestData2::getDestructed() == destructedBefore+1);
        
        // and ours should still be there
        REQUIRE( pGlobalThreadLocal==pData );
    }
    
#endif
    
    
    
    
}

