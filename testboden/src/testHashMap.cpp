#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/HashMap.h>

#include "testCollection.h"

namespace bdn
{
namespace test
{

template<>
struct CollectionElementOrderUndefined_< bdn::HashMap<int, double> >
{
	enum
	{
		value = 1
	};
};

template<>
struct CollectionElementOrderUndefined_< bdn::HashMap<TestCollectionElement_OrderedComparable_, TestCollectionElement_UnorderedComparable_> >
{
	enum
	{
		value = 1
	};
};


}
}



using namespace bdn;
using namespace bdn::test;


template< typename COLL >
static void _testHashMapToString(COLL& coll)
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
			expected += bdn::toString(el.first)+": "+bdn::toString(el.second);

			first = false;
		}

		expected += " }";
	}

	REQUIRE( coll.toString() == expected );
}


template<typename KeyType, typename ValType, typename... ConstructArgs>
static void testHashMap(
    std::initializer_list< std::pair<const KeyType,ValType> > initElList,
	std::list< std::pair<const KeyType,ValType> > expectedInitElOrder,
    std::initializer_list< std::pair<const KeyType,ValType> > newElList,
    std::function< bool(const std::pair<const KeyType,ValType>&) > isMovedRemnant,
    std::pair<const KeyType,ValType> expectedConstructedEl,
    ConstructArgs... constructArgs )
{
	SECTION("test traits")
	{
		REQUIRE( ! (bdn::test::CollectionSupportsBiDirIteration_< HashMap<KeyType, ValType> >::value) );
	}

    SECTION("construct")
    {
        std::list< std::pair<const KeyType, ValType> > expectedElements;

        SECTION("iterators")
        {
            SECTION("empty")
            {
                HashMap<KeyType, ValType> coll( newElList.begin(), newElList.begin() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("non-empty")
            {
                HashMap<KeyType, ValType> coll( newElList.begin(), newElList.end() );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

               _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("copy")
        {
            SECTION("HashMap")
            {
                HashMap<KeyType, ValType> src( newElList );

                HashMap<KeyType, ValType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("std::unordered_map")
            {
                std::unordered_map<KeyType, ValType> src( newElList );

                HashMap<KeyType, ValType> coll( (const std::unordered_map<KeyType, ValType>&) src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("move")
        {
            SECTION("HashMap")
            {
                HashMap<KeyType,ValType> src( newElList );

                HashMap<KeyType,ValType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }

            SECTION("std::unordered_map")
            {
                std::unordered_map<KeyType,ValType> src( newElList );

                HashMap<KeyType,ValType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }
        }

        SECTION("initializer_list")
        {
            HashMap<KeyType,ValType> coll( newElList );

            expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
            _verifyGenericCollectionReadOnly( coll, expectedElements );
        }
    }
    HashMap<KeyType,ValType> coll;

    SECTION("empty")
    {
        _verifyGenericCollection(
            coll,
            std::list< std::pair<const KeyType,ValType> >({}),
            newElList,
            isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)... );

        SECTION("prepareForSize")
            _testGenericCollectionPrepareForSize(coll);        

		SECTION("toString")
			_testHashMapToString(coll);
    }

    SECTION("non-empty")
    {
        for(auto& el: initElList)
            coll.add( el );

        _verifyGenericCollection(
            coll,
            std::list< std::pair<const KeyType, ValType> >(expectedInitElOrder),
            newElList,
            isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)... );        
        
        SECTION("prepareForSize")
            _testGenericCollectionPrepareForSize(coll);        

		SECTION("toString")
			_testHashMapToString(coll);

		
		SECTION("add(key, value)")
		{
			typename HashMap<KeyType,ValType>::Element elToAdd = *newElList.begin();
			std::list<typename HashMap<KeyType,ValType>::Element> newExpectedElementList = expectedInitElOrder;
			newExpectedElementList.push_back( elToAdd );

			coll.add( elToAdd.first, elToAdd.second);

			_verifyGenericCollectionReadOnly( coll, newExpectedElementList );			
		}

		
		SECTION("add ops with existing key")
		{
			SECTION("single el")
			{
				KeyType key = newElList.begin()->first;
				ValType val1 = newElList.begin()->second;
				auto secondNewElIt = newElList.begin();
				++secondNewElIt;
				ValType val2 = secondNewElIt->second;

				std::list<typename HashMap<KeyType,ValType>::Element> expectedElementList = expectedInitElOrder;
				expectedElementList.push_back( std::make_pair(key, val2) );

				SECTION("add(pair)")
				{
					coll.add( std::make_pair(key, val1) );

					// this should overwrite the value
					coll.add( std::make_pair(key, val2) );			

					_verifyGenericCollectionReadOnly( coll, expectedElementList );
				}

				SECTION("add(key, value)")
				{
					coll.add( key, val1);

					// this should overwrite the value
					coll.add( key, val2);

					_verifyGenericCollectionReadOnly( coll, expectedElementList );
				}

				SECTION("addNew")
				{
					std::pair<const KeyType,ValType>& initialMapEl = coll.addNew( key, val1);
					REQUIRE( _isCollectionElementEqual(initialMapEl, std::make_pair(key, val1) ) );
					
					std::pair<const KeyType,ValType>& mapEl =  coll.addNew( key, val2);
					REQUIRE( _isCollectionElementEqual(mapEl, std::make_pair(key, val2) ) );

					_verifyGenericCollectionReadOnly( coll, expectedElementList );
				}
			}

			SECTION("iterators")
			{
				std::list<typename HashMap<KeyType,ValType>::Element> expectedElementList = expectedInitElOrder;
				expectedElementList.insert(expectedElementList.end(), newElList.begin(), newElList.end() );

				// first add the items with a default-constructed value
				for( auto& el: newElList )
					coll.add( el.first, ValType() );

				// then add again with the real values				
				coll.addSequence( newElList.begin(), newElList.end() );

				_verifyGenericCollectionReadOnly( coll, expectedElementList );
			}
		}

    }    
}


template<typename KeyType, typename ValType >
static void _testMapFind(
	std::initializer_list< std::pair<const KeyType, ValType> > elList,
	const std::pair<const KeyType, ValType>& elNotInList )
{
	_testCollectionFind< HashMap<KeyType, ValType> >( elList, elNotInList);

	SECTION("find functions use key AND value")
	{
		HashMap<KeyType, ValType> coll(elList);
	
		SECTION("contains(element)")
		{
			SECTION("key in list, value in list")
				REQUIRE( coll.contains( std::make_pair(elList.begin()->first, elList.begin()->second)  ) );
			SECTION("key in list, value not in list")
				REQUIRE( ! coll.contains( std::make_pair(elList.begin()->first, elNotInList.second ) ) );			
			SECTION("key not in list, value in list")
				REQUIRE( ! coll.contains( std::make_pair(elNotInList.first, elList.begin()->second ) ) );
			SECTION("key not in list, value not in list")
				REQUIRE( ! coll.contains( std::make_pair(elNotInList.first, elNotInList.second ) ) );
		}

		SECTION("find(element)")
		{
			SECTION("key in list, value in list")
				REQUIRE( coll.find( std::make_pair(elList.begin()->first, elList.begin()->second)  ) != coll.end() );
			SECTION("key in list, value not in list")
				REQUIRE( coll.find( std::make_pair(elList.begin()->first, elNotInList.second ) ) == coll.end() );			
			SECTION("key not in list, value in list")
				REQUIRE( coll.find( std::make_pair(elNotInList.first, elList.begin()->second ) ) == coll.end() );
			SECTION("key not in list, value not in list")
				REQUIRE( coll.find( std::make_pair(elNotInList.first, elNotInList.second ) ) == coll.end() );
		}

		SECTION("findAndRemove(element)")
		{
			SECTION("generic tests")
				_verifyCollectionFindAndRemove(coll, elList, elNotInList );

			SECTION("key in list, value in list")
			{
				auto toFind = std::make_pair(elList.begin()->first, elList.begin()->second);

				REQUIRE( coll.contains(toFind) );	// sanity check
				size_t sizeBefore = coll.getSize();

				coll.findAndRemove(toFind);

				REQUIRE( !coll.contains(toFind) );
				REQUIRE( coll.getSize() == sizeBefore-1 );
			}

			SECTION("key in list, value not in list")
			{
				auto toFind = std::make_pair(elList.begin()->first, elNotInList.second );

				REQUIRE( !coll.contains(toFind) );	// sanity check
				size_t sizeBefore = coll.getSize();

				coll.findAndRemove(toFind);

				REQUIRE( !coll.contains(toFind) );
				REQUIRE( coll.getSize() == sizeBefore );
			}

			SECTION("key not in list, value in list")
			{
				auto toFind = std::make_pair(elNotInList.first, elList.begin()->second );

				REQUIRE( !coll.contains(toFind) );	// sanity check
				size_t sizeBefore = coll.getSize();

				coll.findAndRemove(toFind);

				REQUIRE( !coll.contains(toFind) );
				REQUIRE( coll.getSize() == sizeBefore );
			}

			SECTION("key not in list, value not in list")
			{
				auto toFind = std::make_pair(elNotInList.first, elNotInList.second );

				REQUIRE( !coll.contains(toFind) );	// sanity check
				size_t sizeBefore = coll.getSize();

				coll.findAndRemove(toFind);

				REQUIRE( !coll.contains(toFind) );
				REQUIRE( coll.getSize() == sizeBefore );
			}
		}
	}

	SECTION("contains(key)")
	{		
		HashMap<KeyType, ValType> coll(elList);

		REQUIRE( coll.contains( elList.begin()->first ) );
		REQUIRE( !coll.contains( elNotInList.first ) );
	}

	SECTION("findAll(key)")
	{	
		SECTION("non const")
		{
			_testCollectionFindXWithCaller< HashMap<KeyType, ValType> >(
				elList,
				elNotInList,
				[elNotInList](HashMap<KeyType, ValType>& coll, const typename HashMap<KeyType, ValType>::Element& elToFind)
				{
					return coll.findAll( elToFind.first );
				} );
		}

		SECTION("const")
		{
			_testCollectionFindXWithCaller< HashMap<KeyType, ValType> >(
				elList,
				elNotInList,
				[elNotInList](HashMap<KeyType, ValType>& coll, const typename HashMap<KeyType, ValType>::Element& elToFind)
				{
					return ((const HashMap<KeyType, ValType>&)coll).findAll( elToFind.first );
				} );
		}
	}

	SECTION("findAndRemove(key)")
	{
		HashMap<KeyType, ValType> coll(elList);

		KeyType keyToFind = elList.begin()->first;

		// sanity check
		REQUIRE( coll.contains(keyToFind) );

		size_t sizeBefore = coll.getSize();

		coll.findAndRemove(keyToFind);

		REQUIRE( !coll.contains(keyToFind) );

		REQUIRE( coll.getSize() == sizeBefore-1 );
	}
}


template<class KeyType, class ValType>
static void _testMapFindAndRemove( std::initializer_list< std::pair<const KeyType, ValType> > elList, const std::pair<const KeyType, ValType>& elNotInList)
{
    SECTION("empty")
    {
        HashMap<KeyType,ValType> coll;

        SECTION("findAndRemove")    
        {
            coll.findAndRemove(elNotInList);
            _verifyGenericCollectionReadOnly( coll, {} );
        }

        SECTION("findCustomAndRemove")
        {
            coll.findCustomAndRemove(
                [elNotInList](const HashMap<KeyType, ValType>::Iterator& it )
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
                    HashMap<KeyType,ValType> coll(elList);

                    size_t      sizeBefore = coll.getSize();

                    std::list< std::pair<const KeyType, ValType> > expectedElements( elList );
                    expectedElements.remove( el );
                    
                    SECTION("findAndRemove")  
                    {
                        coll.findAndRemove(el);

                        REQUIRE( coll.getSize() == sizeBefore-1 );
                        _verifyGenericCollectionReadOnly( coll, expectedElements );
                    }

                    SECTION("findCustomAndRemove")  
                    {
                        coll.findCustomAndRemove(
                            [el](const HashMap<KeyType, ValType>::Iterator& it)
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
            HashMap<KeyType,ValType> coll(elList);

            std::list< std::pair<const KeyType, ValType> > expectedElements( elList );

            SECTION("findAndRemove")
            {
				SECTION("key and value different")
				{
					coll.findAndRemove(elNotInList);
					_verifyGenericCollectionReadOnly( coll, expectedElements );
				}

				if(elList.begin() != elList.end() )
				{
					SECTION("key different")
					{
						coll.findAndRemove( std::make_pair(elNotInList.first, elList.begin()->second) );
						_verifyGenericCollectionReadOnly( coll, expectedElements );
					}

					SECTION("value different")
					{
						coll.findAndRemove( std::make_pair( elList.begin()->first, elNotInList.second) );
						_verifyGenericCollectionReadOnly( coll, expectedElements );
					}
				}
            }

            SECTION("findCustomAndRemove")
            {
                coll.findCustomAndRemove(
                    [](const HashMap<KeyType, ValType>::Iterator& it)
                    {
                        return false;
                    } );
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("all match")
        {
            HashMap<KeyType,ValType> coll(elList);

            SECTION("findCustomAndRemove")
            {
                coll.findCustomAndRemove(
                    [](const HashMap<KeyType, ValType>::Iterator& it)
                    {
                        return true;
                    } );

                _verifyGenericCollectionReadOnly( coll, {} );
            }
        }
    }
}

TEST_CASE("HashMap")
{
    SECTION("key simple type")
    {
		std::unordered_map<int, double> tempMapForOrder{
				{17, 1.7},
				{42, 4.2},
				{3, 0.3} };

		std::list< std::pair<const int, double> > expectedInitElOrder( tempMapForOrder.begin(), tempMapForOrder.end() );


        testHashMap<int, double>(
			{
				{17, 1.7},
				{42, 4.2},
				{3, 0.3} },
			expectedInitElOrder,
			{
				{100, 10.0},
				{101, 10.1},
				{102, 10.2} },
            [](const std::pair<const int,double>& el)
            {
                return true;
            },
			{345, 34.5},
			345, 34.5 );       

		_testMapFind< int, double >(
			{
				{3, 0.3},
				{17, 1.7},
				{42, 4.2} },
			{100, 10.0} );

		_testMapFindAndRemove< int, double >(
			{
				{3, 0.3},
				{17, 1.7},
				{42, 4.2} },
			{100, 10.0} );

    }

	SECTION("addSequence with compatible but different type")
	{
		// String objects can be implicitly converted to std::string.
		// Passing a source sequence with String objects to a collection with std::string
		// elements should work.

		HashMap< int, std::string > coll;

		SECTION("initializer_list")
		{
			coll.addSequence( { {1, String("hello")}, {2, String("world") } } );
			_verifyGenericCollectionReadOnly( coll, { {1, std::string("hello") }, {2, std::string("world") } } );
		}

		SECTION("std::list")
		{
			coll.addSequence( std::list< std::pair<int, String> >( { {1, String("hello")}, {2, String("world") } } ) );
			_verifyGenericCollectionReadOnly( coll, { {1, std::string("hello") }, {2, std::string("world") } } );
		}
	}

    SECTION("key complex type")
    {
        testHashMap<TestCollectionElement_OrderedComparable_, TestCollectionElement_UnorderedComparable_>(
            { 
				{ TestCollectionElement_OrderedComparable_(17, 117), {333,333} },
				{ TestCollectionElement_OrderedComparable_(42, 142), {111,111} },
				{ TestCollectionElement_OrderedComparable_(3, 103), {222,222} },
            },
			{ 
				{ TestCollectionElement_OrderedComparable_(3, 103), {222,222} },
				{ TestCollectionElement_OrderedComparable_(17, 117), {333,333} },
				{ TestCollectionElement_OrderedComparable_(42, 142), {111,111} },
            },
			{ 
				{ TestCollectionElement_OrderedComparable_(103, 203), {555,555} },
				{ TestCollectionElement_OrderedComparable_(117, 217), {666,666} },
				{ TestCollectionElement_OrderedComparable_(142, 242), {444,444} },
            },
            [](const std::pair<const TestCollectionElement_OrderedComparable_, TestCollectionElement_UnorderedComparable_>& el)
            {
				// the first pair element (key) is only moved if the element was not yet in the list. So
				// we do not require that the key was moved away.
                return // el.first._a==-2 && el.first._b==-2 
						el.second._a==-2 && el.second._b==-2;
            },
			{ std::make_pair( TestCollectionElement_OrderedComparable_(345, 456), TestCollectionElement_UnorderedComparable_(345, 345)) },
			std::make_pair(TestCollectionElement_OrderedComparable_(345, 456), TestCollectionElement_UnorderedComparable_(345, 345))  );


		_testMapFind< TestCollectionElement_OrderedComparable_, TestCollectionElement_UnorderedComparable_ >(
			{ 
				{ TestCollectionElement_OrderedComparable_(3, 103), {222,222} },
				{ TestCollectionElement_OrderedComparable_(17, 117), {333,333} },
				{ TestCollectionElement_OrderedComparable_(42, 142), {111,111} },
            },
			{ TestCollectionElement_OrderedComparable_(103, 203), {555,555} } );

		_testMapFindAndRemove< TestCollectionElement_OrderedComparable_, TestCollectionElement_UnorderedComparable_ >(
			{ 
				{ TestCollectionElement_OrderedComparable_(3, 103), {222,222} },
				{ TestCollectionElement_OrderedComparable_(17, 117), {333,333} },
				{ TestCollectionElement_OrderedComparable_(42, 142), {111,111} },
            },
			{ TestCollectionElement_OrderedComparable_(103, 203), {555,555} } );
    }
}
