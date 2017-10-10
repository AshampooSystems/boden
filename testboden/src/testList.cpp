#include <bdn/init.h>
#include <bdn/test.h>

#include "testCollection.h"

#include <bdn/List.h>

using namespace bdn;
using namespace bdn::test;



template<typename ElType, typename... ConstructArgs>
static void testList(
    std::initializer_list<ElType> initElList,
    std::initializer_list<ElType> newElList,
    std::function< bool(const ElType&) > isMovedRemnant,
    ElType expectedConstructedEl,
    ConstructArgs... constructArgs )
{
    SECTION("construct")
    {
        std::list<ElType> expectedElements;

        SECTION("n copies")
        {
            SECTION("0")
            {
                List<ElType> coll( 0, *newElList.begin() );
                
                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }

            SECTION("3")
            {
                List<ElType> coll( 3, *newElList.begin() );

                for(int i=0; i<3; i++)
                    expectedElements.push_back( *newElList.begin() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("n default constructed")
        {
            SECTION("0")
            {
                List<ElType> coll( 0 );
                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }

            SECTION("3")
            {
                List<ElType> coll( 3 );

                for(int i=0; i<3; i++)
                    expectedElements.push_back( ElType() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("iterators")
        {
            SECTION("empty")
            {
                List<ElType> coll( newElList.begin(), newElList.begin() );
                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }

            SECTION("non-empty")
            {
                List<ElType> coll( newElList.begin(), newElList.end() );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("copy")
        {
            SECTION("List")
            {
                List<ElType> src( newElList );

                List<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }

            SECTION("std::list")
            {
                std::list<ElType> src( newElList );

                List<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, expectedElements );
            }
        }

        SECTION("move")
        {
            SECTION("List")
            {
                List<ElType> src( newElList );

                List<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifySequenceCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }

            SECTION("std::list")
            {
                std::list<ElType> src( newElList );

                List<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                _verifySequenceCollectionReadOnly( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }
        }

        SECTION("initializer_list")
        {
            List<ElType> coll( newElList );

            expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
            _verifySequenceCollectionReadOnly( coll, expectedElements );
        }
    }

    List<ElType> coll;

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


TEST_CASE("List")
{
    SECTION("simple type")
    {
        testList<int>(
            {17, 42, 3},
            {100, 101, 102},
            [](const int& el)
            {
                return true;
            },
            345,
            345 );       

        _testCollectionFind< List<int> >( {17, 42, 17, 3}, 88 );
        _testCollectionSort< List<int> >( {17, 42, 17, 3}, {3, 17, 17, 42} );
    }

    SECTION("complex type")
    {
        SECTION("ordered")
        {
            testList<TestCollectionElement_OrderedComparable_>(
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

            _testCollectionFind< List<TestCollectionElement_OrderedComparable_> >(
                { TestCollectionElement_OrderedComparable_(17, 117),
                    TestCollectionElement_OrderedComparable_(42, 142),
                    TestCollectionElement_OrderedComparable_(17, 117),
                    TestCollectionElement_OrderedComparable_(3, 103),
                },
                TestCollectionElement_OrderedComparable_(400, 401) );

            _testCollectionSort< List<TestCollectionElement_OrderedComparable_> >(
                { TestCollectionElement_OrderedComparable_(17, 1),
                    TestCollectionElement_OrderedComparable_(42, 142),
                    TestCollectionElement_OrderedComparable_(17, 2),
                    TestCollectionElement_OrderedComparable_(3, 103),
                },
                { TestCollectionElement_OrderedComparable_(3, 103),
                    TestCollectionElement_OrderedComparable_(17, 1),
                    TestCollectionElement_OrderedComparable_(17, 2),
                    TestCollectionElement_OrderedComparable_(42, 142),
                } );
        }

        SECTION("unordered comparable")
        {
            testList<TestCollectionElement_UnorderedComparable_>(
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

            _testCollectionFind< List<TestCollectionElement_UnorderedComparable_> >(
                { TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(42, 142),                
                    TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(3, 103),
                },
                TestCollectionElement_UnorderedComparable_(400, 401) );

            // cannot sort because not ordered
        }

        SECTION("unordered uncomparable")
        {
            testList<TestCollectionElement_UnorderedUncomparable_>(
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

            // cannot use List::find, since elements are not comparable
        }
    }
}

