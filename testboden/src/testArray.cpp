#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Array.h>

using namespace bdn;


template<class CollType>
void verifyEmptyCollection(CollType& coll)
{
    SECTION("isEmpty")
        REQUIRE( coll.isEmpty() );

    SECTION("size")
    {
        REQUIRE( coll.getSize()==0 );
        REQUIRE( coll.size()==0 );
    }

    SECTION("iterators")
    {
        REQUIRE( coll.begin() == coll.end() );
        REQUIRE( coll.constBegin() == coll.constBegin() );
        REQUIRE( coll.reverseBegin() == coll.reverseEnd() );
        REQUIRE( coll.constReverseBegin() == coll.constReverseEnd() );        
    }

    SECTION("maxSize")
    {
        REQUIRE( coll.getMaxSize() >= 0x7ffffff );
        REQUIRE( coll.max_size() == coll.getMaxSize() );
    }
}

template<class CollType>
void verifyEmptySequence(CollType& coll)
{
    verifyEmptyCollection(coll);
    
    SECTION("getFirst")
        REQUIRE_THROWS_AS( coll.getFirst(), OutOfRangeError );

    SECTION("getLast")
        REQUIRE_THROWS_AS( coll.getLast(), OutOfRangeError );
}


template<class ElType, class ItType>
static void verifyIterators(ItType it, ItType end, const std::list<ElType>& expectedElementList )
{
    for( ElType expectedEl: expectedElementList )
    {
        REQUIRE( it!=end );
        REQUIRE( *it == expectedEl );
        ++it;
    }

    REQUIRE( it==end );
}

template<class CollType>
static void verifyCollIteration(CollType& coll, const std::list<typename CollType::ElementType>& expectedElementList)
{
    SECTION("normal")
        verifyIterators<typename CollType::ElementType>( coll.begin(), coll.end(), expectedElementList );

    SECTION("const coll")
        verifyIterators<typename CollType::ElementType>( ((const Array<int>&)coll).begin(), ((const Array<int>&)coll).end(), expectedElementList );

    SECTION("constBegin/End")
        verifyIterators<typename CollType::ElementType>( coll.constBegin(), coll.constEnd(), expectedElementList );

    std::list< typename CollType::ElementType > reversedExpectedElementList( expectedElementList );
    std::reverse( reversedExpectedElementList.begin(), reversedExpectedElementList.end() );
        
    SECTION("reverse normal")
        verifyIterators<typename CollType::ElementType>( coll.reverseBegin(), coll.reverseEnd(), reversedExpectedElementList );

    SECTION("reverse const coll")
        verifyIterators<typename CollType::ElementType>( ((const Array<int>&)coll).reverseBegin(), ((const Array<int>&)coll).reverseEnd(), reversedExpectedElementList  );

    SECTION("constReverseBegin/End")
        verifyIterators<typename CollType::ElementType>( coll.constReverseBegin(), coll.constReverseEnd(), reversedExpectedElementList );
}

template<class CollType>
static void verifyInsertAt(CollType& coll, int insertIndex, std::list<typename CollType::ElementType> expectedElementList, std::list<typename CollType::ElementType> newElList)
{
    std::list<typename CollType::ElementType> newExpectedElementList = expectedElementList;

    typename CollType::Iterator insertIt = coll.begin();
    typename CollType::Iterator expectedInsertIt = newExpectedElementList.begin();
    for(int i=0; i<insertIndex; i++)
    {
        ++insertIt;
        ++expectedInsertIt;
    }

    typename CollType::ElementType elToAdd = newElList.front();

    SECTION("ref")
    {
        coll.insertAt( insertIt, elToAdd );
        newExpectedElementList.insertAt( expectedInsertIt );

        verifySequence( coll, newExpectedElList, newElList, false );
    }

    SECTION("move")
    {
        coll.insertAt( insertIt, std::move(elToAdd) );
        newExpectedElementList.insertAt( expectedInsertIt );

        verifySequence( coll, newExpectedElList, newElList, false );

        // elToAdd should not have the same value anymore
        REQUIRE( elToAdd != newElList.front() );
    }

    SECTION("iterators")
    {
        SECTION("empty")
        {
            coll.insertAt( insertIt, newElList.begin(), newElList.begin() );
            verifySequence( coll, newExpectedElList, newElList, false );
        }

        SECTION("non-empty")
        {
            coll.insertAt( insertIt, newElList.begin(), newElList.end() );
            newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

            verifySequence( coll, newExpectedElList, newElList, false );
        }
    }

    SECTION("initializer_list")
        XXX
}

template<class CollType>
static void verifySequence(CollType& coll, std::list<typename CollType::ElementType> expectedElementList, std::list<typename CollType::ElementType> newElList, bool testModification = true)
{
    SECTION("size")
    {
        REQUIRE( coll.size() == expectedElementList.size() );
        REQUIRE( coll.getSize() == expectedElementList.size() );
    }

    SECTION("maxSize")
    {
        REQUIRE( coll.max_size() >= 0x7fffffff );
        REQUIRE( coll.getMaxSize() >= 0x7fffffff );
    }


    SECTION("empty")
    {
        REQUIRE( coll.empty() == (expectedElementList.size()==0) );
        REQUIRE( coll.isEmpty() == (expectedElementList.size()==0) );
    }

    SECTION("getFirst")
    {
        if(expectedElementList.size() == 0)
            REQUIRE_THROWS_AS( coll.getFirst(), OutOfRangeError );
        else
            REQUIRE( coll.getFirst() == expectedElementList.front() );
    }

    SECTION("getLast")
    {
        if(expectedElementList.size() == 0)
            REQUIRE_THROWS_AS( coll.getLast(), OutOfRangeError );
        else
            REQUIRE( coll.getLast() == expectedElementList.back() );
    }

    SECTION("iteration")
        verifyCollIteration( coll, expectedElementList );
    
    if(testModification)
    {
        std::list<typename CollType::ElementType> newExpectedElementList( expectedElementList );

        SECTION("clear")
        {
            coll.clear();

            verifySequence( coll, {}, newElList, false);
        }
    
        if( !coll.isEmpty() )
        {
            SECTION("removeAt")
            {
                SECTION("first")
                {
                    coll.removeAt( coll.begin() );
                    newExpectedElementList.erase( newExpectedElementList.begin() );                   
                    
                    verifySequence( coll, newExpectedElementList, newElList, false);
                }

                SECTION("last")
                {
                    coll.removeAt( --coll.end() );
                    newExpectedElementList.erase( --newExpectedElementList.end() );                   
                    
                    verifySequence( coll, newExpectedElementList, newElList, false);
                }

                if( coll.getSize() > 1)
                {
                    coll.removeAt( ++coll.begin() );
                    newExpectedElementList.erase( ++newExpectedElementList.begin() );                   
                    
                    verifySequence( coll, newExpectedElementList, newElList, false);
                }
            }
        }

        SECTION("add")
        {
            typename CollType::ElementType elToAdd = newElList.front();

            SECTION("ref")
            {
                coll.add( elToAdd );
                newExpectedElementList.push_back( elToAdd );
                verifySequence( coll, newExpectedElList, newElList, false );
            }

            SECTION("move")
            {
                coll.add( std::move(elToAdd) );
                newExpectedElementList.push_back( elToAdd );
                verifySequence( coll, newExpectedElList, newElList, false );

                // elToAdd should not have the same value anymore
                REQUIRE( elToAdd != newElList.front() );
            }

            SECTION("iterators")
            {
                SECTION("empty")
                {
                    coll.add( newElList.begin(), newElList.begin() );
                    verifySequence( coll, newExpectedElList, newElList, false );
                }

                SECTION("non-empty")
                {
                    coll.add( newElList.begin(), newElList.end() );
                    newExpectedElementList.insert(newExpectedElementList.end(), newElList.begin(), newElList.end() );

                    verifySequence( coll, newExpectedElList, newElList, false );
                }
            }
        }

        SECTION("insertAt")
        {
            SECTION("at begin")
                verifyInsertAt(coll, 0, expectedElList, newElList);

            if(expectedElList.size()>1)
            {
                SECTION("in middle")
                    verifyInsertAt(coll, 1, expectedElList, newElList);
            }

            SECTION("at end")
                verifyInsertAt(coll, expectedElList.size(), expectedElList, newElList);

            insert n*el
        }
    }

}


TEST_CASE("Array")
{
    Array<int> coll;
    
    SECTION("empty")
    {
        verifySequence( coll, {} );
    }

    SECTION("non-empty")
    {
        coll.add( 17 );
        coll.add( 42 );
        coll.add( 3 );

        verifySequence( coll, {17, 42, 3} );
    }

    addNew
        insertNewAt
        insertAtFront/Back
        removeFirst
        removeLast
        setSize
        find
        findCondition
        reverseFind
        reverseFindCondition
        sort
        stableSort
}



