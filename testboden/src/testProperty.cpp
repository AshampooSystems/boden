#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/DefaultProperty.h>

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

	virtual Property<ValType>& operator=(const ValType& val)
    {
        set(val);
        
        return *this;
    }
	
	Notifier<const ReadProperty&> & onChange() override
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
}


template<class PropertyType>
void testIntProperty()
{
	PropertyType prop;

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


