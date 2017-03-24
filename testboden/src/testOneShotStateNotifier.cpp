#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/OneShotStateNotifier.h>

using namespace bdn;

static void verifySame()
{	
}

template<class T1>
static void verifySame(T1 a, T1 b)
{	
	REQUIRE( a==b );
}

template<class T1, class T2>
static void verifySame(T1 a1, T2 a2, T1 b1, T2 b2)
{	
	REQUIRE( a1==b1 );
	REQUIRE( a2==b2 );
}

template<class... ArgTypes>
void testOneShotStateNotifier(ArgTypes... args)
{
	OneShotStateNotifier<ArgTypes...> notifier;

    SECTION("one call")
    {
        int callCount=0;
        notifier += 
			[&callCount, args...](ArgTypes... callArgs)
			{
				verifySame(callArgs..., args...);
				
				callCount++;
			};

        notifier.notify(args...);

        REQUIRE(callCount==1);
    }

    SECTION("multiple calls")
    {
        int callCount=0;
        notifier += 
			[&callCount, args...](ArgTypes... callArgs)
			{
				verifySame(callArgs..., args...);
				
				callCount++;
			};

        notifier.notify(args...);

        REQUIRE(callCount==1);

        REQUIRE_THROWS_PROGRAMMING_ERROR( notifier.notify(args...) );
    }

    SECTION("late subscription")
    {
        notifier.notify(args...);

        int callCount=0;
        notifier += 
			[&callCount, args...](ArgTypes... callArgs)
			{
				verifySame(callArgs..., args...);
				
				callCount++;
			};

        // the subscription should have been called immediately.
        REQUIRE(callCount==1);

        REQUIRE_THROWS_PROGRAMMING_ERROR( notifier.notify(args...) );
    }


    SECTION("subscriptions are released after notify")
    {
        P<Base> pDummy = newObj<Base>();

        int callCount=0;
        notifier += 
			[pDummy, &callCount, args...](ArgTypes... callArgs)
			{
				verifySame(callArgs..., args...);
				
				callCount++;
			};

        REQUIRE(pDummy->getRefCount()==2);

        notifier.notify(args...);

        REQUIRE(callCount==1);

        // the reference held by the lambda function should have been released.
        REQUIRE(pDummy->getRefCount()==1);
        
    }
}

TEST_CASE("OneShotStateNotifier")
{
    SECTION("oneArg-String")
	{
		testOneShotStateNotifier<String>(String("hello"));
	}

	SECTION("noArgs")
	{
		testOneShotStateNotifier<>();
	}

	SECTION("oneArg-int")
	{
		testOneShotStateNotifier<int>(42);
	}

   

	SECTION("twoArgs")
	{
		testOneShotStateNotifier<int, String>(42, String("hello") );
	}


    SECTION("paramIsReference")
	{
        String inputString("hello");

        SECTION("standard tests")
        {
		    testOneShotStateNotifier<String&>( inputString );
        }

        SECTION("param changed between notify and late subscribe")
        {
            OneShotStateNotifier<String&> notifier;

            notifier.notify(inputString);

            int callCount=0;
            notifier += 
			    [&callCount, &inputString](String& paramString)
			    {
                    // we must get a reference to the exact same string object
                    REQUIRE( &paramString == &inputString);
				
				    callCount++;
			    };

            // the subscription should have been called immediately.
            REQUIRE(callCount==1);
        }
	}

}


