#include <bdn/init.h>
#include <bdn/test.h>

#include "testCollection.h"

#include <bdn/Set.h>

using namespace bdn;
using namespace bdn::test;


template<typename ElType, typename... ConstructArgs>
static void testSet(
    std::initializer_list<ElType> initElList,
	std::initializer_list<ElType> expectedInitElOrder,
    std::initializer_list<ElType> newElList,
    std::function< bool(const ElType&) > isMovedRemnant,
    ElType expectedConstructedEl,
    ConstructArgs... constructArgs )
{
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
    }    
}


template<class CollType>
static void _verifySetFind( CollType& coll, std::initializer_list< typename CollType::Element > elList, const typename CollType::Element & elNotInList)
{
    SECTION("found")
    {
        SECTION("find")
        {        
            for(auto& el: elList)
            {
                auto it = coll.find(el);

                REQUIRE( it!=coll.end() );
                REQUIRE( _isCollectionElementEqual( *it, el) );
            }
        }

        SECTION("contains")
        {
            for(auto& el: elList)
                REQUIRE( coll.contains(el) );
        }

        SECTION("findCondition")
        {
            for(auto& el: elList)
            {
                auto it = coll.findCondition( 
                    [el]( const typename CollType::Element& setEl )
                    {
                        return _isCollectionElementEqual(el, setEl);
                    } );

                REQUIRE( it!=coll.end() );
                REQUIRE( _isCollectionElementEqual( *it, el) );
            }
        }
    }

    SECTION("not found")
    {
        SECTION("find")
            REQUIRE( coll.find(elNotInList) == coll.end() );

        SECTION("contains")
            REQUIRE( !coll.contains(elNotInList) );

        SECTION("findCondition")
        {
            REQUIRE( coll.findCondition( 
                        []( const typename CollType::Element& setEl )
                        {
                            return false;
                        } )
                    == coll.end() );
        }
    }
}

template<class ElType>
static void _testSetFind( std::initializer_list<ElType> elList, const ElType& elNotInList )
{
    SECTION("empty")
    {
        Set<ElType> coll;

        SECTION("normal")
            _verifySetFind( coll, {}, *elList.begin() );

        SECTION("const")
            _verifySetFind( (const Set<ElType>&)coll, {}, *elList.begin() );
    }

    SECTION("not empty")
    {
        Set<ElType> coll(elList);

        SECTION("normal")
            _verifySetFind(coll, elList, elNotInList);

        SECTION("const")
            _verifySetFind( (const Set<ElType>&)coll , elList, elNotInList);
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

        SECTION("findConditionAndRemove")
        {
            coll.findConditionAndRemove(
                [elNotInList](const ElType& el)
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

                    SECTION("findConditionAndRemove")  
                    {
                        coll.findConditionAndRemove(
                            [el](const ElType& setEl)
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
            Set<ElType> coll(elList);

            std::list< ElType> expectedElements( elList );
            expectedElements.sort();

            SECTION("findAndRemove")
            {
                coll.findAndRemove(elNotInList);
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }

            SECTION("findConditionAndRemove")
            {
                coll.findConditionAndRemove(
                    [](const ElType& setEl)
                    {
                        return false;
                    } );
                _verifyGenericCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("all match")
        {
            Set<ElType> coll(elList);

            SECTION("findConditionAndRemove")
            {
                coll.findConditionAndRemove(
                    [](const ElType& setEl)
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

        _testSetFind<int>( {17, 42, 3}, 78 );

        _testSetFindAndRemove<int>( {17, 42, 3}, 78 );

        //_testCollectionFind< Set<int> >( {17, 42, 17, 3}, 88 );        
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

            _testSetFind<TestCollectionElement_OrderedComparable_>(
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
		/*
        SECTION("unordered comparable")
        {
            testSet<TestCollectionElement_UnorderedComparable_>(
                { TestCollectionElement_UnorderedComparable_(17, 117),
                  TestCollectionElement_UnorderedComparable_(42, 142),
                  TestCollectionElement_UnorderedComparable_(3, 103)
                },
                { TestCollectionElement_UnorderedComparable_(100, 201),
                  TestCollectionElement_UnorderedComparable_(102, 202),
                  TestCollectionElement_UnorderedComparable_(103, 203)
                },
                [](const TestCollectionElement_UnorderedComparable_& el)
                {
                    return el._a==-2 && el._b==-2;
                },
                TestCollectionElement_UnorderedComparable_(345, 456),
                345, 456 );

            /*_testCollectionFind< Set<TestCollectionElement_UnorderedComparable_> >(
                { TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(42, 142),                
                    TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(3, 103),
                },
                TestCollectionElement_UnorderedComparable_(400, 401) );*/
		/*
        }*/
		/*
        SECTION("unordered uncomparable")
        {
            testSet<TestCollectionElement_UnorderedUncomparable_>(
                { TestCollectionElement_UnorderedUncomparable_(17, 117),
                  TestCollectionElement_UnorderedUncomparable_(42, 142),
                  TestCollectionElement_UnorderedUncomparable_(3, 103)
                },
                { TestCollectionElement_UnorderedUncomparable_(100, 201),
                  TestCollectionElement_UnorderedUncomparable_(102, 202),
                  TestCollectionElement_UnorderedUncomparable_(103, 203)
                },
                [](const TestCollectionElement_UnorderedUncomparable_& el)
                {
                    return el._a==-2 && el._b==-2;
                },
                TestCollectionElement_UnorderedUncomparable_(345, 456),
                345, 456 );

            // cannot use Set::find, since elements are not comparable
        }*/
    }
}

