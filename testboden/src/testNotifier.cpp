#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/DefaultNotifier.h>
#include <bdn/DanglingFunctionError.h>

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
void testNotifier(ArgTypes... args)
{
	Notifier<ArgTypes...> notifier;

	bool	 called = false;
	P<INotifierSubControl> pSub;

	SECTION("subscribe")
		pSub = notifier.subscribe(
			[&called, args...](ArgTypes... callArgs)
			{
				verifySame(callArgs..., args...);
				
				called = true;
			} );

	SECTION("subscribeParamless")
		pSub = notifier.subscribeParamless([&called](){ called = true; } );




	class Listener : public Base
	{
	public:
		Listener(bool* pCalled, std::function<void(ArgTypes...)> argVerifier)
		{
			_pCalled = pCalled;
			_argVerifier = argVerifier;
		}

		void onNotify(ArgTypes... args)
		{
			_argVerifier(args...);

			*_pCalled = true;			
		}
			
		void onNotifyVoid()
		{
			*_pCalled = true;
		}

		bool* _pCalled;
		std::function<void(ArgTypes...)> _argVerifier;
	};

	Listener l(
		&called,
		[args...](ArgTypes... callArgs)
		{
			verifySame(callArgs..., args...);		
		});

	notifier.notify(std::forward<ArgTypes>(args)...);

	REQUIRE(called);

	pSub->unsubscribe();
	called = false;

	notifier.notify(std::forward<ArgTypes>(args)...);

	// the subscription should have been deleted again
	REQUIRE( !called );
}

TEST_CASE("Notifier")
{
	SECTION("noArgs")
	{
		testNotifier<>();
	}

	SECTION("oneArg")
	{
		testNotifier<int>(42);
	}

	SECTION("twoArgs")
	{
		testNotifier<int, String>(42, String("hello") );
	}

	SECTION("multipleSubscriptions")
	{
		Notifier<int> notifier;
		bool called1 = false;
		bool called2 = false;

		P<INotifierSubControl> pSub1 = notifier.subscribe([&called1](int){called1 = true;} );
		P<INotifierSubControl> pSub2 = notifier.subscribe([&called2](int){called2 = true;} );

		notifier.notify(42);

		REQUIRE( called1 );
		REQUIRE( called2 );

		called1 = false;
		called2 = false;

		SECTION("unsubFirst")
		{
            pSub1->unsubscribe();
			
			notifier.notify(42);

			REQUIRE( !called1 );
			REQUIRE( called2 );

            // unsubscribing again should have no effect
            called1 = false;
		    called2 = false;

            pSub1->unsubscribe();
			
			notifier.notify(42);

			REQUIRE( !called1 );
			REQUIRE( called2 );
		}

		SECTION("unsubscribeSecond")
		{
			pSub2->unsubscribe();
			
			notifier.notify(42);

			REQUIRE( called1 );
			REQUIRE( !called2 );
		}


        SECTION("deleteFirst")
		{
            pSub1 = nullptr;

            // deleting the control object should not affect the subscription
			
			notifier.notify(42);

			REQUIRE( called1 );
			REQUIRE( called2 );
		}

        SECTION("deleteSecond")
		{
            pSub2 = nullptr;

            // deleting the control object should not affect the subscription
			
			notifier.notify(42);

			REQUIRE( called1 );
			REQUIRE( called2 );
		}
	}

	SECTION("notifierDeletedBeforeSubControl")
	{
		P<INotifierSubControl> pSub;

		{
			Notifier<> notifier;
			
			pSub = notifier.subscribe( [](){} );
		}

        SECTION("unsubscribe called")
        {
            pSub->unsubscribe();
        }

        SECTION("unsubscribe not called")
        {
            // do nothing
        }

		pSub = nullptr;		
	}

    SECTION("DanglingFunctionError")
    {
        Notifier<>  notifier;
        int         callCount = 0;
			
		P<INotifierSubControl> pSub = notifier.subscribe(
            [&callCount]()
            {
                callCount++;
                throw DanglingFunctionError();
            } );

        SECTION("subcontrol deleted before notify")
        {
            pSub = nullptr;
        }

        SECTION("subcontrol still exists")
        {
            // do nothing
        }

        // this should NOT cause an exception
        notifier.notify();

        REQUIRE( callCount==1 );

        // the function should have been removed and should not be called again
        notifier.notify();
        // so callCount should still be 1
        REQUIRE( callCount==1 );
    }

    SECTION("unsubscribe during callback")
    {
        Notifier<>  notifier;
        int         callCount = 0;
        P<INotifierSubControl> pSub;
			
		pSub = notifier.subscribe(
            [&pSub, &callCount]()
            {
                callCount++;
                pSub->unsubscribe();
            } );

        // should not crash or cause an exception
        notifier.notify();

        // sanity check
        REQUIRE( callCount==1 );

        // should not be called again
        notifier.notify();

        // so callCount should still be 1
        REQUIRE( callCount==1 );
    }

    SECTION("unsubscribe during callback then DanglingException")
    {
        Notifier<>  notifier;
        int         callCount = 0;
        P<INotifierSubControl> pSub;
			
		pSub = notifier.subscribe(
            [&pSub, &callCount]()
            {
                callCount++;
                pSub->unsubscribe();
                throw DanglingFunctionError();
            } );

        // should not crash or cause an exception
        notifier.notify();

        // sanity check
        REQUIRE( callCount==1 );

        // should not be called again
        notifier.notify();

        // so callCount should still be 1
        REQUIRE( callCount==1 );
    }


    SECTION("unsubscribe next func during callback")
    {
        Notifier<>  notifier;
        int         callCount = 0;
        int         callCount2 = 0;
        P<INotifierSubControl> pSub;
        P<INotifierSubControl> pSub2;
			
		pSub = notifier.subscribe(
            [&pSub2, &callCount]()
            {
                callCount++;
                // unsubscribe the second one
                pSub2->unsubscribe();
            } );


        pSub2 = notifier.subscribe(
            [&callCount2]()
            {
                callCount2++;
            } );


        // should not crash or cause an exception
        notifier.notify();

        // the first function should have been called
        REQUIRE( callCount==1 );

        // the second function should NOT have been called
        REQUIRE( callCount2==0 );
               

        // notify again
        notifier.notify();

        // again, first one should have been called, second not
        REQUIRE( callCount==2 );
        REQUIRE( callCount2==0 );        
    }

    SECTION("unsubscribe func following next during callback")
    {
        Notifier<>  notifier;
        int         callCount = 0;
        int         callCount2 = 0;
        int         callCount3 = 0;
        P<INotifierSubControl> pSub;
        P<INotifierSubControl> pSub2;
        P<INotifierSubControl> pSub3;
			
		pSub = notifier.subscribe(
            [&pSub3, &callCount]()
            {
                callCount++;
                // unsubscribe the third one
                pSub3->unsubscribe();
            } );


        pSub2 = notifier.subscribe(
            [&callCount2]()
            {
                callCount2++;
            } );


        pSub3 = notifier.subscribe(
            [&callCount3]()
            {
                callCount3++;
            } );


        // should not crash or cause an exception
        notifier.notify();

        // the first and second functions should have been called
        REQUIRE( callCount==1 );
        REQUIRE( callCount2==1 );

        // the third function should NOT have been called
        REQUIRE( callCount3==0 );               

        // notify again
        notifier.notify();

        // again, first and second should have been called, third not
        REQUIRE( callCount==2 );
        REQUIRE( callCount2==2 );   
        REQUIRE( callCount3==0 );          
    }

    SECTION("unsubscribeAll")
    {
        Notifier<>  notifier;
        int         callCount=0;

        notifier.subscribe(
            [&callCount]()
            {
                callCount++;
            } );


        notifier.subscribe(
            [&callCount]()
            {
                callCount++;
            } );

        notifier.unsubscribeAll();

        notifier.notify();

        // none of the functions should have been called
        REQUIRE( callCount==0 );

        // subscribe another one and notify again. Then it should be called.
        notifier.subscribe(
            [&callCount]()
            {
                callCount++;
            } );

        notifier.notify();

        REQUIRE( callCount==1 );        
    }


    SECTION("use control after unsubscribeAll")
    {
        Notifier<>  notifier;
        int         callCount=0;

        P<INotifierSubControl> pControl = notifier.subscribe(
            [&callCount]()
            {
                callCount++;
            } );

        notifier.unsubscribeAll();

        notifier.notify();

        // none of the functions should have been called
        REQUIRE( callCount==0 );

        // using the control object should not have any effect
        pControl->unsubscribe();    

        REQUIRE( callCount==0 );

        pControl = nullptr;
    }
}


