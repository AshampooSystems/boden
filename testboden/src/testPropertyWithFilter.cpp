#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/PropertyWithFilter.h>

#include <bdn/test/property.h>

using namespace bdn;


TEST_CASE("PropertyWithFilter")
{
    SECTION("generic property tests")
    {
        SECTION("String")
	    {
		    bdn::test::testProperty<PropertyWithFilter<String>, String>(
			    []() { return newObj<PropertyWithFilter<String> >(); },
			    bdn::test::stringValueGenerator );
	    }

	    SECTION("int")
	    {
		    bdn::test::testProperty<PropertyWithFilter<int>, int>(
			    []() { return newObj<PropertyWithFilter<int> >(); },
			    bdn::test::numberValueGenerator<int> );
	    }
    }

    SECTION("filtering")
    {
        PropertyWithFilter<int> prop;

        SECTION("modified value is stored")
        {
            prop.setFilter(
                [](const int& val)
                {
                    return val+1;
                } );

            prop = 17;

            REQUIRE( prop.get()==18 );            
        }

        SECTION("subscribers get modified value")
        {
            int notifiedVal = -1;

            prop.setFilter(
                [](const int& val)
                {
                    return val+1;
                } );
            auto sub = prop.subscribeVoid(
                [&notifiedVal](const int& val)
                {
                    notifiedVal=val;
                } );

            prop = 17;

            REQUIRE( notifiedVal==18 );
        }

        SECTION("exception prevents value change")
        {
            prop = 42;
        
            prop.setFilter(
                [](const int& val)
                {
                    throw 123;
                } );

            REQUIRE_THROWS_AS( prop = 17, int);         

            REQUIRE( prop.get() == 42 );
        }

        SECTION("exception prevents subscriber notification")
        {
            bool notified = false;
        
            prop.setFilter(
                [](const int& val)
                {
                    throw 123;
                } );

            auto sub = prop.subscribeVoid(
                [&notified]()
                {
                    notified = true;                    
                } )

            REQUIRE_THROWS_AS( prop = 17, int);         

            REQUIRE( prop.get() == 42 );

            // subscriber should not have been notified
            REQUIRE( !notified )
        }
    }

}








