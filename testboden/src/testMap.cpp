#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Map.h>



#include "testCollection.h"

using namespace bdn;
using namespace bdn::test;


template<typename KeyType, typename ValType, typename... ConstructArgs>
static void testMap(
    std::initializer_list< std::pair<const KeyType,ValType> > initElList,
	std::initializer_list< std::pair<const KeyType,ValType> > expectedInitElOrder,
    std::initializer_list< std::pair<const KeyType,ValType> > newElList,
    std::function< bool(const std::pair<const KeyType,ValType>&) > isMovedRemnant,
    std::pair<const KeyType,ValType> expectedConstructedEl,
    ConstructArgs... constructArgs )
{
    SECTION("construct")
    {
        std::list< std::pair<const KeyType, ValType> > expectedElements;

        SECTION("iterators")
        {
            SECTION("empty")
            {
                Map<KeyType, ValType> coll( newElList.begin(), newElList.begin() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("non-empty")
            {
                Map<KeyType, ValType> coll( newElList.begin(), newElList.end() );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

               _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("copy")
        {
            SECTION("Map")
            {
                Map<KeyType, ValType> src( newElList );

                Map<KeyType, ValType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("std::map")
            {
                std::map<KeyType, ValType> src( newElList );

                Map<KeyType, ValType> coll( (const std::map<KeyType, ValType>&) src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("move")
        {
            SECTION("Map")
            {
                Map<KeyType,ValType> src( newElList );

                Map<KeyType,ValType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }

            SECTION("std::map")
            {
                std::map<KeyType,ValType> src( newElList );

                Map<KeyType,ValType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifyGenericCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }
        }

        SECTION("initializer_list")
        {
            Map<KeyType,ValType> coll( newElList );

            expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
            _verifyGenericCollectionReadOnly( coll, expectedElements );
        }
    }
    Map<KeyType,ValType> coll;

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

		
		SECTION("add(key, value)")
		{
			typename Map<KeyType,ValType>::Element elToAdd = *newElList.begin();
			std::list<typename Map<KeyType,ValType>::Element> newExpectedElementList = expectedInitElOrder;
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

				std::list<typename Map<KeyType,ValType>::Element> expectedElementList = expectedInitElOrder;
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
				std::list<typename Map<KeyType,ValType>::Element> expectedElementList = expectedInitElOrder;
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
	_testCollectionFind< Map<KeyType, ValType> >( elList, elNotInList);

	SECTION("find functions use key AND value")
	{
		Map<KeyType, ValType> coll(elList);
	
		SECTION("contains")
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

		SECTION("find")
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
	}

}


template<class KeyType, class ValType>
static void _testMapFindAndRemove( std::initializer_list< std::pair<const KeyType, ValType> > elList, const std::pair<const KeyType, ValType>& elNotInList)
{
    SECTION("empty")
    {
        Map<KeyType,ValType> coll;

        SECTION("findAndRemove")    
        {
            coll.findAndRemove(elNotInList);
            _verifyGenericCollectionReadOnly( coll, {} );
        }

        SECTION("findConditionAndRemove")
        {
            coll.findConditionAndRemove(
                [elNotInList](const std::pair<const KeyType, ValType>& el)
                {
                    return _isCollectionElementEqual(el, elNotInList);
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
                    Map<KeyType,ValType> coll(elList);

                    size_t      sizeBefore = coll.getSize();

                    std::list< std::pair<const KeyType, ValType> > expectedElements( elList );
                    expectedElements.remove( el );
                    
                    SECTION("findAndRemove")  
                    {
                        coll.findAndRemove(el);

                        REQUIRE( coll.getSize() == sizeBefore-1 );
                        _verifyGenericCollectionReadOnly( coll, expectedElements );
                    }

                    SECTION("findConditionAndRemove")  
                    {
                        coll.findConditionAndRemove(
                            [el](const std::pair<const KeyType, ValType>& setEl)
                            {
                                return _isCollectionElementEqual( el, setEl );
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
            Map<KeyType,ValType> coll(elList);

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

            SECTION("findConditionAndRemove")
            {
                coll.findConditionAndRemove(
                    [](const std::pair<const KeyType, ValType>& setEl)
                    {
                        return false;
                    } );
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("all match")
        {
            Map<KeyType,ValType> coll(elList);

            SECTION("findConditionAndRemove")
            {
                coll.findConditionAndRemove(
                    [](const std::pair<const KeyType, ValType>& setEl)
                    {
                        return true;
                    } );

                _verifyGenericCollectionReadOnly( coll, {} );
            }
        }
    }
}

TEST_CASE("Map")
{
    SECTION("key simple type")
    {
        testMap<int, double>(
			{
				{17, 1.7},
				{42, 4.2},
				{3, 0.3} },
			{
				{3, 0.3},
				{17, 1.7},
				{42, 4.2} },
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

    SECTION("key complex type")
    {
        testMap<TestCollectionElement_OrderedComparable_, TestCollectionElement_UnorderedComparable_>(
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
