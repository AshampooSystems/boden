#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/PropertyWithMainThreadDelegate.h>
#include <bdn/Thread.h>

#include <bdn/test/property.h>

using namespace bdn;


template<class ValType>
class TestDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<ValType>::IDelegate
{
public:
	TestDelegate(int setDelayMillis=0)
	{
		_setDelayMillis = setDelayMillis;
	}

	void	set(const ValType& val)
	{
		REQUIRE( Thread::isCurrentMain() );

		if(_setDelayMillis>0)
			Thread::sleepMillis(_setDelayMillis);

		value = val;
		setCount++;
	}
		
	ValType get() const
	{
		REQUIRE( Thread::isCurrentMain() );

		getCount++;

		return value;
	}

	ValType			value;
	int				setCount=0;
	mutable int		getCount=0;

	int				_setDelayMillis = 0;

};

class ChangeCounter : public Base
{
public:

	void changed()
	{
		val++;
	}

	int val = 0;
};

template<typename ValType>
P< PropertyWithMainThreadDelegate<ValType> > createProp()
{
	P<TestDelegate<ValType> >		pDelegate = newObj<TestDelegate<ValType> >();

	return newObj< PropertyWithMainThreadDelegate<ValType> >(pDelegate, ValType());
}


TEST_CASE("PropertyWithMainThreadDelegate")
{	
	SECTION("standardTests")
	{	
		SECTION("String")
		{
			bdn::test::testProperty< PropertyWithMainThreadDelegate<String>, String>(&createProp<String>, &bdn::test::stringValueGenerator);
		}

		SECTION("int")
		{
			bdn::test::testProperty< PropertyWithMainThreadDelegate<int>, int>(&createProp<int>, &bdn::test::numberValueGenerator<int> );
		}
	}

	SECTION("customTests")
	{
		P<ChangeCounter> pChangeCounter = newObj<ChangeCounter>();
	
		SECTION("fromMainThread")
		{	
			P<TestDelegate<String> >							pDelegate = newObj<TestDelegate<String> >();

			pDelegate->value = "bla";

			P< PropertyWithMainThreadDelegate<String> > pProp = newObj< PropertyWithMainThreadDelegate<String> >(pDelegate, "hello");
			PropertyWithMainThreadDelegate<String>&		prop = *pProp;

			// delegate value should have been updated
			REQUIRE( pDelegate->value=="hello" );

			P<IBase> pChangeSub = prop.onChange().subscribeVoidMember<ChangeCounter>(pChangeCounter, &ChangeCounter::changed);

			REQUIRE( pDelegate->getCount==0 );
			REQUIRE( pDelegate->setCount==1 );

			REQUIRE( prop.get()=="hello" );

			REQUIRE( pDelegate->value=="hello" );
			REQUIRE( pDelegate->getCount==0 );
			REQUIRE( pDelegate->setCount==1 );		

			prop = "world";

			REQUIRE( pChangeCounter->val==1 );

			REQUIRE( pDelegate->value=="world" );
			REQUIRE( pDelegate->getCount==0 );
			REQUIRE( pDelegate->setCount==2 );

			REQUIRE( prop.get()=="world" );

			REQUIRE( pDelegate->value=="world" );
			REQUIRE( pDelegate->getCount==0 );
			REQUIRE( pDelegate->setCount==2 );
		
			pDelegate->value = "hurz";

			prop.updateCachedValue();

			REQUIRE( pDelegate->value=="hurz" );
			REQUIRE( pDelegate->getCount==1 );
			REQUIRE( pDelegate->setCount==2 );

			REQUIRE( prop.get()=="hurz" );

			REQUIRE( pChangeCounter->val==2 );

			REQUIRE( pDelegate->value=="hurz" );
			REQUIRE( pDelegate->getCount==1 );
			REQUIRE( pDelegate->setCount==2 );

			// detach delegate
			prop.detachDelegate();

			prop = "hi";

			REQUIRE( pChangeCounter->val==3 );

			// delegate should not be updated
			REQUIRE( pDelegate->value=="hurz" );
			REQUIRE( pDelegate->getCount==1 );
			REQUIRE( pDelegate->setCount==2 );
		}	

		SECTION("fromOtherThread")
		{
			P<TestDelegate<String> >	pDelegate = newObj<TestDelegate<String> >(1000);
			
			P< PropertyWithMainThreadDelegate<String> > pProp = newObj< PropertyWithMainThreadDelegate<String> >(pDelegate, "hello");
			PropertyWithMainThreadDelegate<String>&		prop = *pProp;

			P<IBase> pChangeSub = prop.onChange().subscribeVoidMember<ChangeCounter>(pChangeCounter, &ChangeCounter::changed);

			MAKE_ASYNC_TEST(10);

			Thread::exec(
				[pProp, pDelegate, pChangeCounter, pChangeSub]()
				{
					PropertyWithMainThreadDelegate<String>&		prop = *pProp;

					REQUIRE( pDelegate->getCount==0 );
					REQUIRE( pDelegate->setCount==1 );

					prop = "world";

					REQUIRE(pChangeCounter->val==1);

					// the delegate delay 1000ms. So the delegate value should still
					// be the same right now.
					REQUIRE( pDelegate->value=="hello" );
					REQUIRE( pDelegate->getCount==0 );
					REQUIRE( pDelegate->setCount==1 );

					// but the property should already have the new value
					REQUIRE( prop.get()=="world" );

					Thread::sleepMillis(2000);

					// now the delegate should have been updated.
					REQUIRE( pDelegate->getCount==0 );
					REQUIRE( pDelegate->setCount==2 );
					REQUIRE( pDelegate->value == "world" );

					// now we block the main thread.
					Mutex blockMutex;
					{
						MutexLock blockMutexLock(blockMutex);;
				
						callFromMainThread([&blockMutex]()
						{
							MutexLock lock(blockMutex);					
						});

						// wait a little to ensure that the main thread is really blocked
						Thread::sleepMillis(1000);
				
						// and now we set the property
						prop = "newworld";
						REQUIRE( prop.get()=="newworld" );

						REQUIRE(pChangeCounter->val==2);

						// the delegate should not be updated yet
						REQUIRE( pDelegate->value=="world" );
						REQUIRE( pDelegate->getCount==0 );
						REQUIRE( pDelegate->setCount==2 );

						// now we detach the delegate
						prop.detachDelegate();

						// the property should still be usable normally
						REQUIRE( prop.get()=="newworld" );

						// and release the main thread lock
					}

					// wait a little
					Thread::sleepMillis(2000);

					// delegate should still not be updated.
					REQUIRE( pDelegate->value=="world" );
					REQUIRE( pDelegate->getCount==0 );
					REQUIRE( pDelegate->setCount==2 );

					// but the property should still be usable normally
					REQUIRE( prop.get()=="newworld" );

					prop.set("hi");

					REQUIRE(pChangeCounter->val==3);

					REQUIRE( prop.get()=="hi" );

					// delegate should still not be updated.
					REQUIRE( pDelegate->value=="world" );
					REQUIRE( pDelegate->getCount==0 );
					REQUIRE( pDelegate->setCount==2 );

					END_ASYNC_TEST();
			
				});
		}


		SECTION("fromOtherThreadWithMultiUpdates")
		{
			P<TestDelegate<String> >	pDelegate = newObj<TestDelegate<String> >();
			
			P< PropertyWithMainThreadDelegate<String> > pProp = newObj< PropertyWithMainThreadDelegate<String> >(pDelegate, "hello");
			PropertyWithMainThreadDelegate<String>&		prop = *pProp;
		
			P<IBase> pChangeSub = prop.onChange().subscribeVoidMember<ChangeCounter>(pChangeCounter, &ChangeCounter::changed);

			MAKE_ASYNC_TEST(10);

			Thread::exec(
				[pProp, pDelegate, pChangeCounter, pChangeSub]()
				{
					PropertyWithMainThreadDelegate<String>&		prop = *pProp;

					REQUIRE( pDelegate->getCount==0 );
					REQUIRE( pDelegate->setCount==1 );

					// now we block the main thread.
					Mutex blockMutex;
					{
						MutexLock blockMutexLock(blockMutex);;
				
						callFromMainThread([&blockMutex]()
						{
							MutexLock lock(blockMutex);					
						});

						// wait a little to ensure that the main thread is really blocked
						Thread::sleepMillis(1000);
				
						// and now we set a new value
						prop = "world";
						REQUIRE( prop.get()=="world" );

						REQUIRE(pChangeCounter->val==1);

						// and immediately another new value
						prop = "hi";
						REQUIRE( prop.get()=="hi" );

						REQUIRE(pChangeCounter->val==2);

						// the delegate should not be updated yet
						REQUIRE( pDelegate->value=="hello" );
						REQUIRE( pDelegate->getCount==0 );
						REQUIRE( pDelegate->setCount==1 );

						// now release the main thread
					}

					// wait a little
					Thread::sleepMillis(2000);

					// delegate should have been updated twice.
					// Note that it might seem better if the first update would
					// be skipped if another is pending. However, that would open
					// the risk that frequent updates could cause the delegate to
					// NEVER be updated, since each new pending update would invalidate
					// the previous pending updates. To prevent that we simply let all
					// updates happen.
					REQUIRE( pDelegate->value=="hi" );
					REQUIRE( pDelegate->getCount==0 );
					REQUIRE( pDelegate->setCount==3 );

					REQUIRE(pChangeCounter->val==2);

					END_ASYNC_TEST();
			
				});
		}

		SECTION("updateCachedValue")
		{
			P<TestDelegate<String> >	pDelegate = newObj<TestDelegate<String> >();
			
			P< PropertyWithMainThreadDelegate<String> > pProp = newObj< PropertyWithMainThreadDelegate<String> >(pDelegate, "hello");
			PropertyWithMainThreadDelegate<String>&		prop = *pProp;

			P<IBase> pChangeSub = prop.onChange().subscribeVoidMember<ChangeCounter>(pChangeCounter, &ChangeCounter::changed);

			SECTION("fromMainThread")
			{
				pDelegate->value = "hi";

				prop.updateCachedValue();

				REQUIRE( prop=="hi" );

				REQUIRE(pChangeCounter->val==1);
			}

			SECTION("fetchWhileSetScheduled")
			{
				MAKE_ASYNC_TEST(10);

				Thread::exec(
					[pProp, pDelegate, pChangeCounter, pChangeSub]()
					{
						PropertyWithMainThreadDelegate<String>&		prop = *pProp;

						REQUIRE( pDelegate->getCount==0 );
						REQUIRE( pDelegate->setCount==1 );

						// now we block the main thread.
						Mutex blockMutex;
						{
							MutexLock blockMutexLock(blockMutex);;
				
							callFromMainThread([&blockMutex]()
							{
								MutexLock lock(blockMutex);					
							});

							// wait a little to ensure that the main thread is really blocked
							Thread::sleepMillis(1000);

							// cause the property to schedule a value change
							prop = "world";
							REQUIRE( prop.get()=="world" );

							REQUIRE(pChangeCounter->val==1);

							// delegate should not be affected yet
							REQUIRE( pDelegate->value=="hello" );
							REQUIRE( pDelegate->getCount==0 );
							REQUIRE( pDelegate->setCount==1 );

							// BUT then change the current value of the delegate manually
							pDelegate->value = "hi";

							// and tell the property to fetch the value
							prop.updateCachedValue();

							// now release the main thread
						}

						// wait a little
						Thread::sleepMillis(2000);

						// the delegate should NOT have been updated.
						REQUIRE( pDelegate->value=="hi" );
						REQUIRE( pDelegate->getCount==1 );
						REQUIRE( pDelegate->setCount==1 );

						// instead the property should have gotten the latest delegate value
						REQUIRE( prop.get()=="hi" );

						REQUIRE(pChangeCounter->val==2);

						END_ASYNC_TEST();
			
					});
			}

			SECTION("setWhileFetchScheduled")
			{
				MAKE_ASYNC_TEST(10);

				Thread::exec(
					[pProp, pDelegate, pChangeCounter, pChangeSub]()
					{
						PropertyWithMainThreadDelegate<String>&		prop = *pProp;

						REQUIRE( pDelegate->getCount==0 );
						REQUIRE( pDelegate->setCount==1 );

						// now we block the main thread.
						Mutex blockMutex;
						{
							MutexLock blockMutexLock(blockMutex);;
				
							callFromMainThread([&blockMutex]()
							{
								MutexLock lock(blockMutex);					
							});

							// wait a little to ensure that the main thread is really blocked
							Thread::sleepMillis(1000);

							// change the current value of the delegate manually
							pDelegate->value = "hi";

							// and tell the property to fetch the value.
							// This will be done in the background.
							prop.updateCachedValue();

							REQUIRE( prop.get()=="hello" );

							// then change the value of the property
							prop = "world";

							REQUIRE(pChangeCounter->val==1);

							// now release the main thread
						}

						// wait a little
						Thread::sleepMillis(2000);

						// the delegate should have been updated to the value that was set.
						REQUIRE( pDelegate->value=="world" );
						REQUIRE( pDelegate->getCount==0 );
						REQUIRE( pDelegate->setCount==2 );

						// and the property should still have the new value
						REQUIRE( prop.get()=="world" );

						// no further change notification should have happened
						REQUIRE(pChangeCounter->val==1);

						END_ASYNC_TEST();
			
					});
			}
		}
	}
}




