#include <bdn/init.h>
#include <bdn/test.h>

#include "testCollection.h"

#include <bdn/Set.h>

using namespace bdn;
using namespace bdn::test;



template< typename COLL >
static void _testSetToString(COLL& coll)
{	
	String expected;
		
	if(coll.isEmpty())
		expected = "{}";
	else
	{
		expected = "{ ";

		bool first = true;
		for( auto& el: coll )
		{
			if(!first)
				expected += ",\n  ";
			expected += bdn::toString(el);

			first = false;
		}

		expected += " }";
	}

	REQUIRE( coll.toString() == expected );
}


template<typename ElType, typename... ConstructArgs>
static void testSet(
    std::initializer_list<ElType> initElList,
	std::initializer_list<ElType> expectedInitElOrder,
    std::initializer_list<ElType> newElList,
    std::function< bool(const ElType&) > isMovedRemnant,
    ElType expectedConstructedEl,
    ConstructArgs... constructArgs )
{
	SECTION("test traits")
	{
		REQUIRE( CollectionSupportsBiDirIteration_< Set<ElType> >::value );
	}

    SECTION("construct")
    {
        std::list<ElType> expectedElements;

        SECTION("iterators")
        {
            SECTION("empty")
            {
                Set<ElType> coll( newElList.begin(), newElList.begin() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("non-empty")
            {
                Set<ElType> coll( newElList.begin(), newElList.end() );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("copy")
        {
            SECTION("Set")
            {
                Set<ElType> src( newElList );

                Set<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("std::set")
            {
                std::set<ElType> src( newElList );

                Set<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("move")
        {
            SECTION("Set")
            {
                Set<ElType> src( newElList );

                Set<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }

            SECTION("std::set")
            {
                std::set<ElType> src( newElList );

                Set<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }
        }

        SECTION("initializer_list")
        {
            Set<ElType> coll( newElList );

            expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
            _verifyGenericCollectionReadOnly( coll, expectedElements );
        }
    }

    Set<ElType> coll;

    SECTION("empty")
    {
        _verifyGenericCollection(
            coll,
            std::list<ElType>({}),
            newElList,
            isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)... );

        SECTION("prepareForSize")
            _testGenericCollectionPrepareForSize(coll);        

		SECTION("toString")
            _testSetToString(coll);        
    }

    SECTION("non-empty")
    {
        for(auto& el: initElList)
            coll.add( el );

        _verifyGenericCollection(
            coll,
            std::list<ElType>(expectedInitElOrder),
            newElList,
            isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)... );        
        
        SECTION("prepareForSize")
            _testGenericCollectionPrepareForSize(coll);     

		SECTION("toString")
            _testSetToString(coll);        
    }    
}




template<class ElType>
static void _testSetFindAndRemove( std::initializer_list< ElType > elList, const ElType& elNotInList)
{
    SECTION("empty")
    {
        Set<ElType> coll;

        SECTION("findAndRemove")    
        {
            coll.findAndRemove(elNotInList);
            _verifyGenericCollectionReadOnly( coll, {} );
        }

        SECTION("findCustomAndRemove")
        {
            coll.findCustomAndRemove(
                [elNotInList](const typename Set<ElType>::Iterator& it )
                {
                    return _isCollectionElementEqual(*it, elNotInList);
                } );
            _verifyGenericCollectionReadOnly( coll, {} );
        }        
    }

    SECTION("not empty")
    {
        SECTION("found")
        {
            int elIndex=0;

            for(auto& el: elList)
            {
                SECTION( std::to_string(elIndex) )
                {
                    Set<ElType> coll(elList);

                    size_t      sizeBefore = coll.getSize();

                    std::list< ElType> expectedElements( elList );
                    expectedElements.remove( el );
                    expectedElements.sort();

                    SECTION("findAndRemove")  
                    {
                        coll.findAndRemove(el);

                        REQUIRE( coll.getSize() == sizeBefore-1 );
                        _verifyGenericCollectionReadOnly( coll, expectedElements );
                    }

                    SECTION("findCustomAndRemove")  
                    {
                        coll.findCustomAndRemove(
                            [el](const typename Set<ElType>::Iterator& it )
                            {
                                return _isCollectionElementEqual( el, *it );
                            } );

                        REQUIRE( coll.getSize() == sizeBefore-1 );
                        _verifyGenericCollectionReadOnly( coll, expectedElements );
                    }
                }

                elIndex++;
            }
        }

        SECTION("not found")
        {
            Set<ElType> coll(elList);

            std::list< ElType> expectedElements( elList );
            expectedElements.sort();

            SECTION("findAndRemove")
            {
                coll.findAndRemove(elNotInList);
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("findCustomAndRemove")
            {
                coll.findCustomAndRemove(
                    [](const typename Set<ElType>::Iterator& it )
                    {
                        return false;
                    } );
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("all match")
        {
            Set<ElType> coll(elList);

            SECTION("findCustomAndRemove")
            {
                coll.findCustomAndRemove(
                    [](const typename Set<ElType>::Iterator& it )
                    {
                        return true;
                    } );

                _verifyGenericCollectionReadOnly( coll, {} );
            }
        }
    }

}

TEST_CASE("Set")
{
    SECTION("simple type")
    {
        testSet<int>(
            {17, 42, 3},
			{3, 17, 42},
            {100, 101, 102},
            [](const int& el)
            {
                return true;
            },
            345,
            345 );       

		_testCollectionFind< Set<int> >({17, 42, 3}, 78 );

        _testSetFindAndRemove<int>( {17, 42, 3}, 78 );
    }

	SECTION("addSequence with compatible but different type")
	{
		// String objects can be implicitly converted to std::string.
		// Passing a source sequence with String objects to a collection with std::string
		// elements should work.

		Set< std::string > coll;

		SECTION("initializer_list")
		{
			coll.addSequence( { String("hello"), String("world") } );
			_verifyGenericCollectionReadOnly( coll, { std::string("hello"), std::string("world") } );
		}

		SECTION("std::list")
		{
			coll.addSequence( std::list<String>( { String("hello"), String("world") } ) );
			_verifyGenericCollectionReadOnly( coll, { std::string("hello"), std::string("world") } );
		}
	}

    SECTION("complex type")
    {
        SECTION("ordered")
        {
            testSet<TestCollectionElement_OrderedComparable_>(
                { TestCollectionElement_OrderedComparable_(17, 117),
                  TestCollectionElement_OrderedComparable_(42, 142),
                  TestCollectionElement_OrderedComparable_(3, 103)
                },
				{ TestCollectionElement_OrderedComparable_(3, 103),
				  TestCollectionElement_OrderedComparable_(17, 117),
                  TestCollectionElement_OrderedComparable_(42, 142),                  
                },
                { TestCollectionElement_OrderedComparable_(100, 201),
                  TestCollectionElement_OrderedComparable_(102, 202),
                  TestCollectionElement_OrderedComparable_(103, 203)
                },
                [](const TestCollectionElement_OrderedComparable_& el)
                {
                    return el._a==-2 && el._b==-2;
                },
                TestCollectionElement_OrderedComparable_(345, 456),
                345, 456 );

            _testCollectionFind< Set<TestCollectionElement_OrderedComparable_> >(
                { TestCollectionElement_OrderedComparable_(17, 117),
                  TestCollectionElement_OrderedComparable_(42, 142),
                  TestCollectionElement_OrderedComparable_(3, 103)
                },
                TestCollectionElement_OrderedComparable_(400, 401) );

            _testSetFindAndRemove<TestCollectionElement_OrderedComparable_>(
                { TestCollectionElement_OrderedComparable_(17, 117),
                  TestCollectionElement_OrderedComparable_(42, 142),
                  TestCollectionElement_OrderedComparable_(3, 103)
                },
                TestCollectionElement_OrderedComparable_(400, 401) );
        }
    }
}

