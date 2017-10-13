#include <bdn/init.h>
#include <bdn/test.h>

#include "testCollection.h"

#include <bdn/Set.h>

using namespace bdn;
using namespace bdn::test;


template<typename ElType, typename... ConstructArgs>
static void testSet(
    std::initializer_list<ElType> initElList,
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
                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }

            SECTION("non-empty")
            {
                Set<ElType> coll( newElList.begin(), newElList.end() );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("copy")
        {
            SECTION("Set")
            {
                Set<ElType> src( newElList );

                Set<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }

            SECTION("std::set")
            {
                std::set<ElType> src( newElList );

                Set<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("move")
        {
            SECTION("Set")
            {
                Set<ElType> src( newElList );

                Set<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifySequenceCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }

            SECTION("std::set")
            {
                std::set<ElType> src( newElList );

                Set<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifySequenceCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }
        }

        SECTION("initializer_list")
        {
            Set<ElType> coll( newElList );

            expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
            _verifySequenceCollectionReadOnly( coll, expectedElements );
        }
    }

    Set<ElType> coll;

    SECTION("empty")
    {
        _verifySequenceCollection(
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

        _verifySequenceCollection(
            coll,
            std::list<ElType>(initElList),
            newElList,
            isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)... );        
        
        SECTION("prepareForSize")
            _testGenericCollectionPrepareForSize(coll);        
    }    
}
/*
template<class CollType>
static void _verifyFindAndRemove(CollType& coll, typename CollType::Element elNotInList)
{
    std::list< ElementInfo<typename CollType::Iterator> > origInfo = _getCollElementInfo(coll);

    SECTION("no match")
    {
        SECTION("find value")
        {
            coll.findAndRemove( elNotInList );
        }

        SECTION("check function")
        {
            coll.findConditionAndRemove( 
                [](const CollType::Element& el)
                {
                    return false;
                } );
        }

        REQUIRE( coll.getSize() == origInfo.size() );

        _verifyElementRange(coll.begin(), coll.end(), origInfo.begin(), origInfo.end() );
    }

    if(!origInfo.empty())
    {
        SECTION("single match")
        {
            // the second element is only once in the list (no duplicates)

            typename CollType::Element toFind = *++coll.begin();

            SECTION("find value")
                coll.findAndRemove( toFind );

            SECTION("check function")
            {   
                coll.findConditionAndRemove( 
                    [toFind](const CollType::Element& el)
                    {
                        return el==toFind;
                    } );
            }

            REQUIRE( coll.getSize() == origInfo.size()-1 );

            // second element should have been removed
            _verifyElementRange(coll.begin(), ++coll.begin(), origInfo.begin(), ++origInfo.begin() );
            _verifyElementRange(++coll.begin(), coll.end(), ++(++origInfo.begin()), origInfo.end() );
        }

        SECTION("multiple matches")
        {
            // the first and third element are equal
            typename CollType::Element toFind = *coll.begin();

            SECTION("find value")
                coll.findAndRemove( toFind );

            SECTION("check function")
            {   
                coll.findConditionAndRemove( 
                    [toFind](const CollType::Element& el)
                    {
                        return el==toFind;
                    } );
            }
                        
            REQUIRE( coll.getSize() == origInfo.size()-2 );

            // first and third should be removed

            // the first result element should be the original second
            _verifyElementRange(coll.begin(), ++coll.begin(), ++origInfo.begin(), ++(++origInfo.begin()) );

            // then the fourth and later should follow
            _verifyElementRange(++coll.begin(), coll.end(), ++(++(++origInfo.begin())), origInfo.end() );
        }
    }
}


template<typename ElType>
static void _testFindAndRemove(std::initializer_list<ElType> elList, ElType elNotInList)
{
    SECTION("empty")
    {
        Set<ElType> coll;
        _verifyFindAndRemove(coll, elNotInList);
    }

    SECTION("not empty")
    {
        Set<ElType> coll(elList);
        _verifyFindAndRemove(coll, elNotInList);
    }
}
*/

TEST_CASE("Set")
{
    SECTION("simple type")
    {
        testSet<int>(
            {17, 42, 3},
            {100, 101, 102},
            [](const int& el)
            {
                return true;
            },
            345,
            345 );       

        _testCollectionFind< Set<int> >( {17, 42, 17, 3}, 88 );        
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
                    TestCollectionElement_OrderedComparable_(17, 117),
                    TestCollectionElement_OrderedComparable_(3, 103),
                },
                TestCollectionElement_OrderedComparable_(400, 401) );
        }

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

            _testCollectionFind< Set<TestCollectionElement_UnorderedComparable_> >(
                { TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(42, 142),                
                    TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(3, 103),
                },
                TestCollectionElement_UnorderedComparable_(400, 401) );

        }

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
        }
    }
}

