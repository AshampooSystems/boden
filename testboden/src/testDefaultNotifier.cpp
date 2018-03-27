#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ThreadSafeNotifier.h>
#include <bdn/DanglingFunctionError.h>

using namespace bdn;

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
void testNotifierAfterSubscribe(
	P< ThreadSafeNotifier<ArgTypes...> >   pNotifier,
	P< DefaultNotifierTestData >        pTestData,
	ArgTypes... args)
{
	class Listener : public Base
	{
	public:
		Listener(DefaultNotifierTestData* pTestData, std::function<void(ArgTypes...)> argVerifier)
		{
			_pTestData = pTestData;
			_argVerifier = argVerifier;
		}

		void onNotify(ArgTypes... args)
		{
			_argVerifier(args...);

			_pTestData->called1 = true;			
		}
			
		void onNotifyVoid()
		{
			_pTestData->called1 = true;
		}

		P<DefaultNotifierTestData>          _pTestData;
		std::function<void(ArgTypes...)>    _argVerifier;
	};

	Listener l(
		pTestData,
		[args...](ArgTypes... callArgs)
		{
			verifySame(callArgs..., args...);		
		});

	SECTION("notify")
	{
		pNotifier->notify(std::forward<ArgTypes>(args)...);

		// should have been called immediately
		REQUIRE( pTestData->called1 );

		// unsubscribe and try again
		pTestData->pSub1->unsubscribe();
		pTestData->called1 = false;

		pNotifier->notify(std::forward<ArgTypes>(args)...);
		
		// the subscription should have been deleted and no call should have been made
		REQUIRE( !pTestData->called1 );
	}

	SECTION("postNotification")
	{
		pNotifier->postNotification(std::forward<ArgTypes>(args)...);

		// nothing should have been called yet
		REQUIRE( !pTestData->called1 );

		CONTINUE_SECTION_WHEN_IDLE_WITH(
			std::bind(
				[pNotifier, pTestData](ArgTypes... args)
				{
					REQUIRE(pTestData->called1);

					// unsubscribe and try again

					pTestData->pSub1->unsubscribe();
					pTestData->called1 = false;

					pNotifier->postNotification(std::forward<ArgTypes>(args)...);

					CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData)
					{
						// the subscription should have been deleted and no call should have been made
						REQUIRE( !pTestData->called1 );
					};
				},
				std::forward<ArgTypes>(args)... ) );
	}
	
}


template<class... ArgTypes>
void testNotifier(ArgTypes... args)
{
	P< ThreadSafeNotifier<ArgTypes...> >   pNotifier = newObj<ThreadSafeNotifier<ArgTypes...>>();
    P< DefaultNotifierTestData >        pTestData = newObj<DefaultNotifierTestData>();
	
	SECTION("subscribe")
	{
		pTestData->pSub1 = pNotifier->subscribe(
			[pTestData, args...](ArgTypes... callArgs)
			{
				verifySame(callArgs..., args...);
				
				pTestData->called1 = true;
			} );

		testNotifierAfterSubscribe(pNotifier, pTestData, std::forward<ArgTypes>(args)...);
	}

	SECTION("subscribeParamless")
	{
		pTestData->pSub1 = pNotifier->subscribeParamless(
            [pTestData]()
            {
                pTestData->called1 = true;
            } );

		testNotifierAfterSubscribe(pNotifier, pTestData, std::forward<ArgTypes>(args)...);
	}

}


static void testDefaultNotifierDanglingFunctionError(
	P<DefaultNotifierTestData>  pTestData,
	P< ThreadSafeNotifier<> >		pNotifier,
	P<INotifierSubControl>		pSub )
{
	SECTION("notify")
	{	
        // this should NOT cause an exception
        pNotifier->notify();

        REQUIRE( pTestData->callCount1==1 );

        // the function should have been removed and should not be called again
        pNotifier->notify();

		// so callCount1 should still be 1
        REQUIRE( pTestData->callCount1==1 );
	}

	SECTION("postNotification")
	{	
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
}

TEST_CASE("ThreadSafeNotifier")
{
    P<DefaultNotifierTestData>  pTestData = newObj<DefaultNotifierTestData>();

    SECTION("require new alloc")
    {
        REQUIRE_THROWS_PROGRAMMING_ERROR( {ThreadSafeNotifier<> testNotifier;} );
    }

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
		P<ThreadSafeNotifier<int> >    pNotifier = newObj<ThreadSafeNotifier<int>>();
        	

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
			P< ThreadSafeNotifier<> > pNotifier = newObj< ThreadSafeNotifier<> >();
			
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

    
    SECTION("Notifier deleted before posted notification func call")
    {
        P< ThreadSafeNotifier<> > pNotifier = newObj< ThreadSafeNotifier<> >();
			
        *pNotifier += [pTestData](){ pTestData->callCount1++; };

		pNotifier->postNotification();

        pNotifier = nullptr;

        CONTINUE_SECTION_WHEN_IDLE( pTestData )
        {
            // notification should have been called, even though our last reference
            // to the notifier was released.
            REQUIRE( pTestData->callCount1==1 );
        };
    }

    SECTION("DanglingFunctionError")
    {
        P< ThreadSafeNotifier<> > pNotifier = newObj< ThreadSafeNotifier<> >();
			
		P<INotifierSubControl> pSub = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
                throw DanglingFunctionError();
            } );

        SECTION("subcontrol deleted before notify")
        {
            pSub = nullptr;

			testDefaultNotifierDanglingFunctionError(pTestData, pNotifier, pSub);
        }

        SECTION("subcontrol still exists")
        {
            // do nothing
			testDefaultNotifierDanglingFunctionError(pTestData, pNotifier, pSub);
        }
		
    }

    SECTION("unsubscribe during callback")
    {
        P< ThreadSafeNotifier<> >  pNotifier = newObj< ThreadSafeNotifier<> >();
			
		pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
                pTestData->pSub1->unsubscribe();
            } );

		SECTION("notify")
		{
			pNotifier->notify();

			// sanity check
			REQUIRE( pTestData->callCount1==1 );

			// should not be called again
			pNotifier->notify();
							
			// so callCount1 should still be 1
			REQUIRE( pTestData->callCount1==1 );
		}

		SECTION("postNotification")
		{
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
    }

    SECTION("unsubscribe during callback then DanglingException")
    {
        P< ThreadSafeNotifier<> >  pNotifier = newObj< ThreadSafeNotifier<> >();
			
		pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
                pTestData->pSub1->unsubscribe();
                throw DanglingFunctionError();
            } );
		
		SECTION("notify")
		{
			// should not crash or cause an exception
			pNotifier->notify();

			// sanity check
			REQUIRE( pTestData->callCount1==1 );
            
			// should not be called again
			pNotifier->notify();

			// so callCount1 should still be 1
			REQUIRE( pTestData->callCount1==1 );
		}

		SECTION("postNotification")
		{
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
    }


    SECTION("unsubscribe next func during callback")
    {
        P< ThreadSafeNotifier<> >  pNotifier = newObj< ThreadSafeNotifier<> >();
			
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

		SECTION("notify")
		{
			// should not crash or cause an exception
			pNotifier->notify();

			// the first function should have been called
			REQUIRE( pTestData->callCount1==1 );

			// the second function should NOT have been called
			REQUIRE( pTestData->callCount2==0 );               

			// notify again
			pNotifier->notify();

			// again, first one should have been called, second not
			REQUIRE( pTestData->callCount1==2 );
			REQUIRE( pTestData->callCount2==0 );        
		}

		SECTION("postNotification")
		{
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
    }

    SECTION("unsubscribe func following next during callback")
    {
        P< ThreadSafeNotifier<> >  pNotifier = newObj< ThreadSafeNotifier<> >();
			
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

		SECTION("notify")
		{
			// should not crash or cause an exception
			pNotifier->notify();
				
			// the first and second functions should have been called
			REQUIRE( pTestData->callCount1==1 );
			REQUIRE( pTestData->callCount2==1 );

			// the third function should NOT have been called
			REQUIRE( pTestData->callCount3==0 );               

			// notify again
			pNotifier->notify();

			// again, first and second should have been called, third not
			REQUIRE( pTestData->callCount1==2 );
			REQUIRE( pTestData->callCount2==2 );   
			REQUIRE( pTestData->callCount3==0 );          
		}

		SECTION("postNotification")
		{
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
    }

    SECTION("unsubscribeAll")
    {
        P< ThreadSafeNotifier<> > pNotifier = newObj<ThreadSafeNotifier<>>();

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

		SECTION("notify")
		{
			pNotifier->notify();

			// none of the functions should have been called
			REQUIRE( pTestData->callCount1==0 );

			// subscribe another one and notify again. Then it should be called.
			pNotifier->subscribe(
				[pTestData]()
				{
					pTestData->callCount1++;
				} );

			pNotifier->notify();

			REQUIRE( pTestData->callCount1==1 );        
		}

		SECTION("postNotification")
		{
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
    }


    SECTION("use control after unsubscribeAll")
    {
        P< ThreadSafeNotifier<> > pNotifier = newObj<ThreadSafeNotifier<>>();

        pTestData->pSub1 = pNotifier->subscribe(
            [pTestData]()
            {
                pTestData->callCount1++;
            } );

        pNotifier->unsubscribeAll();

		SECTION("notify")
		{
			pNotifier->notify();

			// none of the functions should have been called
			REQUIRE( pTestData->callCount1==0 );

			// using the control object should not have any effect
			pTestData->pSub1->unsubscribe();    

			REQUIRE( pTestData->callCount1==0 );

			pTestData->pSub1 = nullptr;
		}

		SECTION("postNotification")
		{
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

}


