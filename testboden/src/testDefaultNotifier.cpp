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
	DefaultNotifier<ArgTypes...> notifier;

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

class DefaultNotifierTestData : public Base
{
public:
    bool called1 = false;
    bool called2 = false;

    int  callCount1 = 0;
    int  callCount2 = 0;
    int  callCount3 = 0;

    P<INotifierSubControl> pSub1;
    P<INotifierSubControl> pSub2;
    P<INotifierSubControl> pSub3;
};


TEST_CASE("DefaultNotifier")
{
    P<DefaultNotifierTestData>  pTestData = newObj<DefaultNotifierTestData>();

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
		P<DefaultNotifier<int> >    pNotifier = newObj<DefaultNotifier<int>>();
        	

		pTestData->pSub1 = pNotifier->subscribe([pTestData](int){pTestData->called1 = true;} );
		pTestData->pSub2 = pNotifier->subscribe([pTestData](int){pTestData->called2 = true;} );

		pNotifier->postNotification(42);

        // should not have been called yet
		REQUIRE( !pTestData->called1 );
		REQUIRE( !pTestData->called2 );

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
        {
            // now the functions should have been called
            REQUIRE( pTestData->called1 );
		    REQUIRE( pTestData->called2 );
        
		    pTestData->called1 = false;
		    pTestData->called2 = false;

		    SECTION("unsub first before notification")
		    {
                pTestData->pSub1->unsubscribe();
			
			    pNotifier->postNotification(42);

                CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
                {
			        REQUIRE( !pTestData->called1 );
			        REQUIRE( pTestData->called2 );

                    // unsubscribing again should have no effect
                    pTestData->called1 = false;
		            pTestData->called2 = false;

                    pTestData->pSub1->unsubscribe();
			
			        pNotifier->postNotification(42);

                    CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
                    {
			            REQUIRE( !pTestData->called1 );
			            REQUIRE( pTestData->called2 );
                    };
                };
		    }

            SECTION("unsub first after notification but before call")
		    {
                pNotifier->postNotification(42);

                pTestData->pSub1->unsubscribe();

                CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
                {
			        REQUIRE( !pTestData->called1 );
			        REQUIRE( pTestData->called2 );
                };
		    }

		    SECTION("unsubscribe second")
		    {
			    pTestData->pSub2->unsubscribe();
			
			    pNotifier->postNotification(42);

                CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
                {
			        REQUIRE( pTestData->called1 );
			        REQUIRE( !pTestData->called2 );
                };
		    }


            SECTION("delete first")
		    {
                pTestData->pSub1 = nullptr;

                // deleting the control object should not affect the subscription			
			    pNotifier->postNotification(42);

                CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
                {
			        REQUIRE( pTestData->called1 );
			        REQUIRE( pTestData->called2 );
                };
		    }

            SECTION("delete second")
		    {
                pTestData->pSub2 = nullptr;

                // deleting the control object should not affect the subscription
			
			    pNotifier->postNotification(42);

			    CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
                {
			        REQUIRE( pTestData->called1 );
			        REQUIRE( pTestData->called2 );
                };
		    }
        };
	}

	SECTION("notifier deleted before subControl")
	{
		P<INotifierSubControl> pSub;

		{
			P< DefaultNotifier<> > pNotifier = newObj< DefaultNotifier<> >();
			
			pSub = pNotifier->subscribe( [](){} );
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
        P< DefaultNotifier<> > pNotifier = newObj< DefaultNotifier<> >();
			
		P<INotifierSubControl> pSub = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
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
        pNotifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pSub, pTestData)
        {
            REQUIRE( pTestData->callCount1==1 );

            // the function should have been removed and should not be called again
            pNotifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(pNotifier, pSub, pTestData)
            {
                // so callCount1 should still be 1
                REQUIRE( pTestData->callCount1==1 );
            };
        };
    }

    SECTION("unsubscribe during callback")
    {
        P< DefaultNotifier<> >  pNotifier = newObj< DefaultNotifier<> >();
			
		pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
                pTestData->pSub1->unsubscribe();
            } );

        // should not crash or cause an exception
        pNotifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
        {
            // sanity check
            REQUIRE( pTestData->callCount1==1 );

            // should not be called again
            pNotifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
            {
                // so callCount1 should still be 1
                REQUIRE( pTestData->callCount1==1 );
            };
        };
    }

    SECTION("unsubscribe during callback then DanglingException")
    {
        P< DefaultNotifier<> >  pNotifier = newObj< DefaultNotifier<> >();
			
		pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
                pTestData->pSub1->unsubscribe();
                throw DanglingFunctionError();
            } );

        // should not crash or cause an exception
        pNotifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
        {
            // sanity check
            REQUIRE( pTestData->callCount1==1 );
            
            // should not be called again
            pNotifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
            {
                // so callCount1 should still be 1
                REQUIRE( pTestData->callCount1==1 );
            };
        };
    }


    SECTION("unsubscribe next func during callback")
    {
        P< DefaultNotifier<> >  pNotifier = newObj< DefaultNotifier<> >();
			
		pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
                // unsubscribe the second one
                pTestData->pSub2->unsubscribe();
            } );


        pTestData->pSub2 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount2++;
            } );


        // should not crash or cause an exception
        pNotifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
        {
            // the first function should have been called
            REQUIRE( pTestData->callCount1==1 );

            // the second function should NOT have been called
            REQUIRE( pTestData->callCount2==0 );               

            // notify again
            pNotifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
            {
                // again, first one should have been called, second not
                REQUIRE( pTestData->callCount1==2 );
                REQUIRE( pTestData->callCount2==0 );        
            };
        };
    }

    SECTION("unsubscribe func following next during callback")
    {
        P< DefaultNotifier<> >  pNotifier = newObj< DefaultNotifier<> >();
			
		pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
                // unsubscribe the third one
                pTestData->pSub3->unsubscribe();
            } );


        pTestData->pSub2 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount2++;
            } );


        pTestData->pSub3 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount3++;
            } );


        // should not crash or cause an exception
        pNotifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
        {
            // the first and second functions should have been called
            REQUIRE( pTestData->callCount1==1 );
            REQUIRE( pTestData->callCount2==1 );

            // the third function should NOT have been called
            REQUIRE( pTestData->callCount3==0 );               

            // notify again
            pNotifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
            {
                // again, first and second should have been called, third not
                REQUIRE( pTestData->callCount1==2 );
                REQUIRE( pTestData->callCount2==2 );   
                REQUIRE( pTestData->callCount3==0 );          
            };
        };
    }

    SECTION("unsubscribeAll")
    {
        P< DefaultNotifier<> > pNotifier = newObj<DefaultNotifier<>>();

        pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
            } );


        pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
            } );

        pNotifier->unsubscribeAll();

        pNotifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
        {
            // none of the functions should have been called
            REQUIRE( pTestData->callCount1==0 );

            // subscribe another one and notify again. Then it should be called.
            pNotifier->subscribe(
                [pTestData]()
                {
                    pTestData->callCount1++;
                } );

            pNotifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
            {
                REQUIRE( pTestData->callCount1==1 );        
            };
        };
    }


    SECTION("use control after unsubscribeAll")
    {
        P< DefaultNotifier<> > pNotifier = newObj<DefaultNotifier<>>();

        pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
            } );

        pNotifier->unsubscribeAll();

        pNotifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
        {
            // none of the functions should have been called
            REQUIRE( pTestData->callCount1==0 );

            // using the control object should not have any effect
            pTestData->pSub1->unsubscribe();    

            REQUIRE( pTestData->callCount1==0 );

            pTestData->pSub1 = nullptr;
        };
    }
}


