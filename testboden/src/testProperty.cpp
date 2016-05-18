#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/DefaultProperty.h>
#include <bdn/Thread.h>

#include <random>

using namespace bdn;


template<class ValType>
class TestProperty : public Property<ValType>
{
public:	
	TestProperty()
	{
	}

	~TestProperty()
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
	
	Notifier<const ReadProperty<ValType>&> & onChange() override
	{
		return _onChange;
	}
    
    void bind(ReadProperty<ValType>& sourceProperty) override
	{
        _pBindSourceSubscription = sourceProperty.onChange().template subscribeMember<TestProperty>(this, &TestProperty::set);
        
        set( sourceProperty.get() );
    }    
    
protected:
	ValType									_value;
    
	Notifier<const ReadProperty<ValType>&>	_onChange;
	P<IBase>								_pBindSourceSubscription;

	
};

template<typename PropertyType>
void testStringProperty()
{
	PropertyType prop;

	SECTION("initialState")
		REQUIRE( prop.get()=="" );

	SECTION("assign")
	{
		SECTION("set")
			prop.set("hello");

		SECTION("operator=")
			prop = "hello";

		REQUIRE(prop.get()=="hello");
	}

	SECTION("read")
	{
		prop = "hello";

		SECTION("get")
			REQUIRE( prop.get()=="hello");

		SECTION("cast")
			REQUIRE( (static_cast<String>(prop) )=="hello");

		SECTION("implicit")
		{
			String val = prop;
			REQUIRE( val=="hello" );
		}
		
		SECTION("operator==")
		{
			REQUIRE( prop=="hello");
			REQUIRE( ! ( prop=="world" ) );
		}

		SECTION("operator!=")
		{
			REQUIRE( prop!="world" );
			REQUIRE( ! (prop!="hello") );
		}
	}

	SECTION("bind")
	{
		SECTION("copiesInitialValue")
		{
			SECTION("sameType")
			{
				PropertyType otherProp;

				otherProp = "hello";

				prop.bind(otherProp);

				REQUIRE( prop=="hello" );
			}

			SECTION("otherType")
			{
				TestProperty<String> otherProp;

				otherProp = "hello";

				prop.bind(otherProp);

				REQUIRE( prop=="hello" );
			}			
		}

		SECTION("reactsToChange")
		{
			SECTION("sameType")
			{
				PropertyType otherProp;

				otherProp.bind(prop);

				prop = "hello";

				REQUIRE( otherProp=="hello" );
			}

			SECTION("otherType")
			{
				SECTION("to")
				{
					TestProperty<String> otherProp;

					otherProp.bind(prop);

					prop = "hello";

					REQUIRE( otherProp=="hello" );			
				}

				SECTION("from")
				{
					TestProperty<String> otherProp;

					prop.bind(otherProp);

					otherProp = "hello";

					REQUIRE( prop=="hello" );			
				}
			}
		}

		SECTION("ignoresAssignmentOfEqualValue")
		{
			PropertyType otherProp;

			otherProp = "hello";

			prop.bind(otherProp);

			REQUIRE( prop=="hello");

			// manually change the value of prop (even though it is bound).
			prop = "world";

			// otherProp should not have changed (binding was in one direction only)
			REQUIRE( otherProp=="hello");
			REQUIRE( prop=="world");

			// assign the same value to otherProp
			otherProp = "hello";

			// this should NOT have modified prop, because the value did not change
			REQUIRE( prop=="world");
		}

		SECTION("cycle")
		{
			SECTION("2")
			{
				PropertyType propB;

				prop = "hello";
				propB.bind(prop);

				REQUIRE( propB=="hello");
				REQUIRE( prop=="hello");

				prop.bind(propB);

				REQUIRE( propB=="hello");
				REQUIRE( prop=="hello");

				prop = "world";
				REQUIRE( propB=="world");
				REQUIRE( prop=="world");
			}

			SECTION("3")
			{
				PropertyType propB;
				PropertyType propC;

				prop = "hello";

				propC.bind(propB);
				propB.bind(prop);				

				REQUIRE( propC=="hello");
				REQUIRE( propB=="hello");
				REQUIRE( prop=="hello");

				prop.bind(propC);

				REQUIRE( propC=="hello");
				REQUIRE( propB=="hello");
				REQUIRE( prop=="hello");

				prop = "world";
				REQUIRE( propC=="world");
				REQUIRE( propB=="world");
				REQUIRE( prop=="world");
				
			}
		
		}


		SECTION("sourceDestroyed")
		{
			{
				PropertyType otherProp;
				
				prop.bind(otherProp);
				otherProp = "world";
			}

			REQUIRE( prop=="world");
		}

		SECTION("destDestroyed")
		{
			{
				PropertyType otherProp;
				
				otherProp.bind(prop);
				prop = "world";

				REQUIRE( otherProp=="world");
			}

			// the main thing we test here is that this does not crash because
			// it tries to update non-existing property.
			prop = "hello";
			REQUIRE( prop=="hello");
		}
	}		


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
}


template<class PropertyType>
void testIntProperty()
{
	PropertyType	prop;

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

TEST_CASE("DefaultProperty")
{
	SECTION("string")
		testStringProperty< DefaultProperty<String> >();

	SECTION("int")
		testIntProperty< DefaultProperty<int> >();

}


