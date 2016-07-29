#ifndef BDN_test_property_H_
#define BDN_test_property_H_

#include <bdn/Thread.h>

#include <random>

namespace bdn
{

namespace test
{	

template<class ValType>
class TestProperty_ : public Property<ValType>
{
public:	
	TestProperty_()
	{
	}

	~TestProperty_()
    {
        // ensure that we unsubscribe from the bound property
		// before any of our members get destroyed
        _pBindSourceSubscription = nullptr;
    }

	ValType get() const override
	{
		return _value;
	}

	void set(const ValType& val) override
	{
		if(_value!=val)
		{
			_value = val;
			_onChange.notify(*this);
		}
	}

	virtual Property<ValType>& operator=(const ValType& val) override
    {
        set(val);
        
        return *this;
    }

	virtual Property<ValType>& operator=(const ReadProperty<ValType>& prop) override
    {
        set( prop.get() );
        
        return *this;
    }
	
	Notifier<const ReadProperty<ValType>&> & onChange() const override
	{
		return _onChange;
	}
    
    void bind(ReadProperty<ValType>& sourceProperty) override
	{
        sourceProperty.onChange().template subscribeMember<TestProperty_>(_pBindSourceSubscription, this, &TestProperty_::set);
        
        set( sourceProperty.get() );
    }    
    
protected:
	ValType									_value;
    
	mutable Notifier<const ReadProperty<ValType>&>	_onChange;
	P<IBase>								_pBindSourceSubscription;

	
};

template<class PropertyType, typename ValType>
void _testPropertyBase(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<ValType(int)> valueGeneratorFunc )
{
	P<PropertyType>	pProp = propertyCreatorFunc();
	PropertyType&   prop = *pProp;

	P<PropertyType>	pPropB = propertyCreatorFunc();
	PropertyType&   propB = *pPropB;

	P<PropertyType>	pPropC = propertyCreatorFunc();
	PropertyType&   propC = *pPropC;

	TestProperty_<ValType> propDiffType;

	ValType defaultConstructedVal = ValType();
	ValType val1 = valueGeneratorFunc(0);
	ValType val2 = valueGeneratorFunc(1);

	SECTION("initialState")
		REQUIRE( prop.get()==defaultConstructedVal );

	SECTION("assign")
	{
		SECTION("set")
			prop.set( val1 );

		SECTION("operator=")
			prop = val1;

		REQUIRE(prop.get()==val1);
	}

	SECTION("read")
	{
		prop = val1;

		SECTION("get")
			REQUIRE( prop.get()==val1);

		SECTION("cast")
			REQUIRE( (static_cast<ValType>(prop) )==val1);

		SECTION("implicit")
		{
			ValType val = prop;
			REQUIRE( val==val1 );
		}
		
		SECTION("operator==")
		{
			REQUIRE( prop==val1);
			REQUIRE( ! ( prop==val2 ) );
		}

		SECTION("operator!=")
		{
			REQUIRE( prop!=val2 );
			REQUIRE( ! (prop!=val1) );
		}
	}

	SECTION("bind")
	{
		SECTION("copiesInitialValue")
		{
			SECTION("sameType")
			{
				propB = val1;

				prop.bind(propB);

				REQUIRE( prop==val1 );
			}

			SECTION("otherType")
			{
				propDiffType = val1;

				prop.bind(propDiffType);

				REQUIRE( prop==val1 );
			}			
		}

		SECTION("reactsToChange")
		{
			SECTION("sameType")
			{
				propB.bind(prop);

				prop = val1;

				REQUIRE( propB==val1 );
			}

			SECTION("otherType")
			{
				SECTION("to")
				{
					propDiffType.bind(prop);

					prop = val1;

					REQUIRE( propDiffType==val1 );			
				}

				SECTION("from")
				{
					prop.bind(propDiffType);

					propDiffType = val1;

					REQUIRE( prop==val1 );			
				}
			}
		}

		SECTION("ignoresAssignmentOfEqualValue")
		{
			propB = val1;

			prop.bind(propB);

			REQUIRE( prop==val1);

			// manually change the value of prop (even though it is bound).
			prop = val2;

			// propB should not have changed (binding was in one direction only)
			REQUIRE( propB==val1);
			REQUIRE( prop==val2);

			// assign the same value to propB
			propB = val1;

			// this should NOT have modified prop, because the value did not change
			REQUIRE( prop==val2);
		}

		SECTION("cycle")
		{
			SECTION("2")
			{
				prop = val1;
				propB.bind(prop);

				REQUIRE( propB==val1);
				REQUIRE( prop==val1);

				prop.bind(propB);

				REQUIRE( propB==val1);
				REQUIRE( prop==val1);

				prop = val2;
				REQUIRE( propB==val2);
				REQUIRE( prop==val2);
			}

			SECTION("3")
			{
				prop = val1;

				propC.bind(propB);
				propB.bind(prop);				

				REQUIRE( propC==val1);
				REQUIRE( propB==val1);
				REQUIRE( prop==val1);

				prop.bind(propC);

				REQUIRE( propC==val1);
				REQUIRE( propB==val1);
				REQUIRE( prop==val1);

				prop = val2;
				REQUIRE( propC==val2);
				REQUIRE( propB==val2);
				REQUIRE( prop==val2);
				
			}
		
		}


		SECTION("sourceDestroyed")
		{
			{
				TestProperty_<ValType> tempProp;

				prop.bind(tempProp);
				tempProp = val2;
			}

			REQUIRE( prop==val2);
		}

		SECTION("destDestroyed")
		{
			{
				TestProperty_<ValType> tempProp;
				
				tempProp.bind(prop);
				prop = val2;

				REQUIRE( tempProp==val2);
			}

			// the main thing we test here is that this does not crash because
			// it tries to update non-existing property.
			prop = val1;
			REQUIRE( prop==val1);
		}
	}	

#if BDN_HAVE_THREADS
	
	SECTION("threadSafety")
	{
		std::list< std::future<void> > futureList;

		const int threadCount = 100;

		std::random_device	randDevice;
		std::mt19937		rng( randDevice() );		
		std::uniform_int_distribution<> waitRange(0, 5);

		int waitMillis[threadCount];
		for(int i=0; i<threadCount; i++)
		{
			// every second thread waits a short random time, just to make sure that we get some contention.			
			if((i & 1)==0)
				waitMillis[i] = 0;
			else
				waitMillis[i] = waitRange(rng);
		}

		ValType initialValue = prop;

		std::list< ValType > writeValueList;
		std::list< ValType > readValueList;
		Mutex				 readValueListMutex;

		for(int i=0; i<100; i++)
		{			
			ValType writeVal = valueGeneratorFunc(i);

			writeValueList.push_back(writeVal);

			futureList.push_back( Thread::exec(
				[&prop, &waitMillis, writeVal, &readValueList, &readValueListMutex, i]()
				{
					if(waitMillis[i]!=0)
						Thread::sleepMillis(waitMillis[i]);

					ValType readVal = prop;
					prop = writeVal;
					
					{
						MutexLock lock(readValueListMutex);
						readValueList.push_back(readVal);
					}

				} ) );
		}

		// wait for all threads to finish
		for(std::future<void>& f: futureList)
			f.get();

		ValType lastVal = prop;

		// the value must be one of the ones we put in there. It is somewhat random
		// which one this ends up being.
		REQUIRE_IN( lastVal, writeValueList );

		// also, all the values that we read in the threads must be either the default-constructed
		// one or ones that we wrote.
		std::list<ValType> validReadValues = writeValueList;
		validReadValues.push_back(initialValue);

		for( ValType readVal: readValueList)
		{
			REQUIRE_IN( readVal, validReadValues );			
		}
	}
    
#endif
}



template<class PropertyType, typename ValType>
void testStringProperty(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<ValType(int)> valueGeneratorFunc )
{
	_testPropertyBase(propertyCreatorFunc, valueGeneratorFunc);
    
#if BDN_HAVE_THREADS
		
	SECTION("threadSafeAppend")
	{
		P<PropertyType>	pProp = propertyCreatorFunc();
		PropertyType&   prop = *pProp;

		std::list< std::future<void> > futureList;

		const int threadCount = 100;

		std::random_device	randDevice;
		std::mt19937		rng( randDevice() );		
		std::uniform_int_distribution<> waitRange(0, 5);

		int waitMillis[threadCount];
		for(int i=0; i<threadCount; i++)
		{
			// every second thread waits a short random time, just to make sure that we get some contention.			
			if((i & 1)==0)
				waitMillis[i] = 0;
			else
				waitMillis[i] = waitRange(rng);
		}

		for(int i=0; i<100; i++)
		{			
			futureList.push_back( Thread::exec(
				[&prop, &waitMillis, i]()
				{
					if(waitMillis[i]!=0)
						Thread::sleepMillis(waitMillis[i]);
					prop = prop + (std::to_string(i)+",");
				} ) );
		}

		// wait for all threads to finish
		for(std::future<void>& f: futureList)
			f.get();

		std::vector<int> values;
		String result = prop;
		while(!result.isEmpty())
		{
			String token = result.splitOffToken(",");

			if(token.isEmpty())
			{
				// the last token will be empty.
				REQUIRE(result.isEmpty());
				break;
			}

			int intVal = std::stoi(token.asUtf8());
			values.push_back(intVal);
		}

		std::vector<int> valuesBefore = values;
		std::sort(values.begin(), values.end());

		// if this fails then the values were added in the correct order. That is unlikely
		// given the random waits that happen in the different threads. It suggests a bug
		// in the test or the thread implementation.
		REQUIRE( values!=valuesBefore );

		int prevVal = -1;
		for(int currVal: values)
		{
			// no value twice
			REQUIRE( currVal>prevVal );

			// value in the correct range
			REQUIRE( currVal>=0 );
			REQUIRE( currVal<100);

			// Note that we CAN have missing values. The way the algorithm works does not
			// protect against lost updates!

			prevVal = currVal;
		}

		// there can be missing values. But SOME should have made it through.
		// Note that if this fails then it does not necessarily mean that the Property is
		// implemented incorrectly. It could simply be a case of very bad luck.
		// If this happens then this test might have to be redesigned.
		REQUIRE( values.size()>=20 );
	}
    
#endif
}


template<class PropertyType, typename ValType>
void testNumberProperty(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<ValType(int)> valueGeneratorFunc )
{
	_testPropertyBase(propertyCreatorFunc, valueGeneratorFunc);

	P<PropertyType>	pProp = propertyCreatorFunc();
	PropertyType&   prop = *pProp;

	SECTION("initialValue")
	{
		REQUIRE( prop==0 );
	}

	prop = 42;

	SECTION("*")
	{		
		prop = prop*2;

		REQUIRE( prop==84 );
	}

	SECTION("+")
	{		
		prop = prop+12;

		REQUIRE( prop==54 );
	}
}

template<class PropertyType, typename ValType>
class TestPropertyHelper_
{
public:	
	static void doTest(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<ValType(int)> valueGeneratorFunc )
	{
		_testPropertyBase(propertyCreatorFunc, valueGeneratorFunc);
	}
};

template<class PropertyType>
class TestPropertyHelper_<PropertyType, String>
{
public:	
	static void doTest(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<String(int)> valueGeneratorFunc )
	{
		testStringProperty(propertyCreatorFunc, valueGeneratorFunc);
	}
};

template<class PropertyType>
class TestPropertyHelper_<PropertyType, int>
{
public:	
	static void doTest(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<int(int)> valueGeneratorFunc )
	{
		testNumberProperty(propertyCreatorFunc, valueGeneratorFunc);
	}
};

template<class PropertyType>
class TestPropertyHelper_<PropertyType, int64_t>
{
public:	
	static void doTest(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<int64_t(int)> valueGeneratorFunc )
	{
		testNumberProperty(propertyCreatorFunc, valueGeneratorFunc);
	}
};


template<class PropertyType>
class TestPropertyHelper_<PropertyType, double>
{
public:	
	static void doTest(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<double(int)> valueGeneratorFunc )
	{
		testNumberProperty(propertyCreatorFunc, valueGeneratorFunc);
	}
};


template<class PropertyType, typename ValType>
void testProperty(std::function< P<PropertyType>() > propertyCreatorFunc, std::function<ValType(int)> valueGeneratorFunc )
{
	TestPropertyHelper_<PropertyType, ValType>::doTest(propertyCreatorFunc, valueGeneratorFunc );
}


String stringValueGenerator(int i)
{
	return "v"+String( std::to_string(i) );
}


template<class NumberType>
NumberType numberValueGenerator(int i)
{
	return 42+i;
}

	
}

}

#endif

