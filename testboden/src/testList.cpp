#include <bdn/init.h>
#include <bdn/test.h>

#include "testCollection.h"

#include <bdn/List.h>

using namespace bdn;
using namespace bdn::test;

template<class CollType>
void _verifyIteratorsStillValid(CollType& coll, std::list<typename CollType::Iterator>& itList )
{
    for(auto& it: itList)
    {
        // compare the element pointers to check if the iterator still points
        // to an element in the new list.
        bool foundEl = false;
        for( auto checkIt = coll.begin(); checkIt!=coll.end(); ++checkIt )
        {
            if( &*it == &*checkIt )
            {
                foundEl = true;
                break;
            }
        }

        REQUIRE( foundEl );
    }
}

template<class CollType>
void verifyStealAllAndMergeSorted(CollType& coll, CollType& other)
{
    CollType origColl(coll);
    CollType origOther(other);

    // make a list with all iterators from the two lists
    std::list< typename CollType::Iterator > its;
    std::list< typename CollType::Iterator > otherIts;
    for( auto it = coll.begin(); it!=coll.end(); ++it)
        its.push_back(it);
    for( auto it = other.begin(); it!=other.end(); ++it)
        otherIts.push_back(it);

    SECTION("List&")
        coll.stealAllAndMergeSorted(other);

    SECTION("std::list&")
        coll.stealAllAndMergeSorted( static_cast<std::list<typename CollType::Element, typename CollType::Allocator>&>(other) );

    SECTION("List&&")
        coll.stealAllAndMergeSorted( std::move(other) );

    SECTION("std::list&&")
        coll.stealAllAndMergeSorted( std::move( static_cast<std::list<typename CollType::Element, typename CollType::Allocator>&>(other) ) );

    REQUIRE( other.isEmpty() );

    // verify that the collection is sorted
    for( auto it = coll.begin(); it!=coll.end(); ++it)
    {
        auto nextIt = it;
        ++nextIt;

        if(nextIt!=coll.end())
            REQUIRE( ! (*nextIt < *it) );
    }

    // verify that no iterators have been invalidated
    _verifyIteratorsStillValid( coll, its );
    _verifyIteratorsStillValid( coll, otherIts );

    // verify that all elements from the original lists are still there
    for( auto& el: coll )
    {
        if( !origColl.isEmpty() && _isCollectionElementEqual(el, origColl.getFirst()) )
            origColl.removeFirst();
        else
        {
            REQUIRE( !origOther.isEmpty() );
            REQUIRE( _isCollectionElementEqual(el, origOther.getFirst()) );
            origOther.removeFirst();
        }
    }

    // verify that all elements have been found
    REQUIRE( origOther.isEmpty() );
    REQUIRE( origColl.isEmpty() );
}



template<class CollType, typename ComesBeforeFuncType>
void verifyStealAllAndMergeSorted(CollType& coll, CollType& other, ComesBeforeFuncType comesBefore)
{
    CollType origColl(coll);
    CollType origOther(other);

    // make a list with all iterators from the two lists
    std::list< typename CollType::Iterator > its;
    std::list< typename CollType::Iterator > otherIts;
    for( auto it = coll.begin(); it!=coll.end(); ++it)
        its.push_back(it);
    for( auto it = other.begin(); it!=other.end(); ++it)
        otherIts.push_back(it);

    SECTION("List&")
        coll.stealAllAndMergeSorted(other, comesBefore);

    SECTION("std::list&")
        coll.stealAllAndMergeSorted( static_cast<std::list<typename CollType::Element, typename CollType::Allocator>&>(other), comesBefore );

    SECTION("List&&")
        coll.stealAllAndMergeSorted( std::move(other), comesBefore );

    SECTION("std::list&&")
        coll.stealAllAndMergeSorted( std::move( static_cast<std::list<typename CollType::Element, typename CollType::Allocator>&>(other) ), comesBefore );

    REQUIRE( other.isEmpty() );

    // verify that the collection is sorted
    for( auto it = coll.begin(); it!=coll.end(); ++it)
    {
        auto nextIt = it;
        ++nextIt;

        if(nextIt!=coll.end())
            REQUIRE( ! comesBefore(*nextIt, *it) );
    }

    // verify that no iterators have been invalidated
    _verifyIteratorsStillValid( coll, its );
    _verifyIteratorsStillValid( coll, otherIts );

    // verify that all elements from the original lists are still there
    for( auto& el: coll )
    {
        if( !origColl.isEmpty() && _isCollectionElementEqual(el, origColl.getFirst()) )
            origColl.removeFirst();
        else
        {
            REQUIRE( !origOther.isEmpty() );
            REQUIRE( _isCollectionElementEqual(el, origOther.getFirst()) );
            origOther.removeFirst();
        }
    }

    // verify that all elements have been found
    REQUIRE( origOther.isEmpty() );
    REQUIRE( origColl.isEmpty() );
}



template<class CollType>
void _testStealAllAndMergeSorted(CollType& coll, CollType& other)
{
    SECTION("both sorted")
    {
        SECTION("operator<")
        {
            coll.sort();
            other.sort();

            verifyStealAllAndMergeSorted(coll, other);
        }

        SECTION("custom order")
        {
            auto sortFunc = [](const typename CollType::Element& a, const typename CollType::Element& b)
                {
                    return (b<a);
                };

            coll.sort( sortFunc );
            other.sort( sortFunc );

            verifyStealAllAndMergeSorted(coll, other, sortFunc);
        }
    }

    /* we cannot test the cases where the preconditions (both lists sorted) are
       not met. In these cases some C++ std libs cause debug assertions to be triggered
       and could potentially cause crashes.

    SECTION("left sorted")
    {
        coll.sort();

        _testStealAllAndMergeSorted(coll, other, false);
    }

    SECTION("right sorted")
    {
        other.sort();

        _testStealAllAndMergeSorted(coll, other, false);
    }

    SECTION("none sorted")
    {
        _testStealAllAndMergeSorted(coll, other, false);
    }*/
}




template<typename ElType>
void _testStealAllAndMergeSorted(
    std::initializer_list<ElType> initElList,
    std::initializer_list<ElType> newElList)
{
    List<ElType> coll;
    List<ElType> other;

    SECTION("empty - empty")
        _testStealAllAndMergeSorted(coll, other);

    SECTION("empty - not empty")
    {
        other = newElList;
        _testStealAllAndMergeSorted(coll, other);
    }

    SECTION("not empty - empty")
    {
        coll = initElList;
        _testStealAllAndMergeSorted(coll, other);
    }

    SECTION("not empty - not empty")
    {
        coll = initElList;
        other = newElList;
        _testStealAllAndMergeSorted(coll, other);
    }
}


template<class ItType>
struct ElementInfo
{
    ElementInfo(ItType it)
    {
        _it = it;
        _elPointer = &*it;
        _elValue = *it;
    }

    void verify(ItType it)
    {
        REQUIRE( &*it == _elPointer );
        REQUIRE( _isCollectionElementEqual(*it, _elValue) );
        REQUIRE( &*_it == _elPointer );
    }

    ItType                          _it;
    typename ItType::value_type*    _elPointer;
    typename ItType::value_type     _elValue;
};


template<typename ItType>
static void _verifyElementRange(
    ItType beginIt,
    ItType endIt,
    typename std::list< ElementInfo<ItType> >::iterator expectedBeginIt,
    typename std::list< ElementInfo<ItType> >::iterator expectedEndIt)
{
    ItType currIt = beginIt;
    std::list< ElementInfo<ItType> >::iterator expectedCurrIt = expectedBeginIt;

    while(currIt!=endIt)
    {
        REQUIRE( expectedCurrIt!=expectedEndIt );

        expectedCurrIt->verify( currIt );

        ++currIt;
        ++expectedCurrIt;
    }

    REQUIRE( expectedCurrIt == expectedEndIt );
}


template<class CollType>
static std::list< ElementInfo<typename CollType::Iterator> > _getCollElementInfo( CollType& coll )
{
    std::list< ElementInfo<typename CollType::Iterator> > infoList;

    for( auto it = coll.begin(); it!=coll.end(); ++it)
        infoList.push_back( it );

    return infoList;
}

template<class CollType>
static typename std::list< ElementInfo<typename CollType::Iterator> >::iterator _toElementInfoIt(
    CollType& coll,
    typename CollType::Iterator it,
    std::list< ElementInfo<typename CollType::Iterator> >& infoList )
{
    if(it==coll.end())
        return infoList.end();

    for( auto infoIt = infoList.begin();
         infoIt!=infoList.end();
         ++infoIt)
    {
        if(infoIt->_elPointer == &*it)
            return infoIt;
    }

    REQUIRE( false );

    return infoList.end();
}



template<typename CollType>
static void _verifyStealAndInsert( CollType& coll, CollType& other, typename CollType::Iterator insertPos)
{
    std::list< ElementInfo<typename CollType::Iterator> > info = _getCollElementInfo(coll);
    std::list< ElementInfo<typename CollType::Iterator> > otherInfo = _getCollElementInfo(other);

    size_t insertIndex = std::distance(coll.begin(), insertPos);

    std::list< ElementInfo<typename CollType::Iterator> >::iterator infoInsertPos = _toElementInfoIt(coll, insertPos, info);

    size_t                      transferCount = 0;
    size_t                      transferBeginIndex = 0;

    SECTION("stealAllAndInsertAt")
    {
        SECTION("List&")
            coll.stealAllAndInsertAt(insertPos, other);

        SECTION("List&&")
            coll.stealAllAndInsertAt(insertPos, std::move(other) );

        SECTION("std::list&")
            coll.stealAllAndInsertAt(insertPos, static_cast<std::list<typename CollType::Element>& >(other) );

        SECTION("std::list&&")
            coll.stealAllAndInsertAt(insertPos, std::move(static_cast<std::list<typename CollType::Element>& >(other)) );

        transferCount = otherInfo.size();
        transferBeginIndex = 0;
    }       


    SECTION("stealSectionAndInsertAt")
    {
        SECTION("empty transfer")
        {
            SECTION("List&")
                coll.stealSectionAndInsertAt(insertPos, other, other.begin(), other.begin() );

            SECTION("List&&")
                coll.stealSectionAndInsertAt(insertPos, std::move(other), other.begin(), other.begin()  );

            SECTION("std::list&")
                coll.stealSectionAndInsertAt(insertPos, static_cast<std::list<typename CollType::Element>& >(other), other.begin(), other.begin()  );

            SECTION("std::list&&")
                coll.stealSectionAndInsertAt(insertPos, std::move(static_cast<std::list<typename CollType::Element>& >(other)), other.begin(), other.begin()  );

            transferCount = 0;
            transferBeginIndex = 0;
        }

        SECTION("partial transfer")
        {
            transferBeginIndex = 0;

            typename CollType::ConstIterator transferEndIt = other.begin();
            transferCount = other.size() / 2;
            std::advance( transferEndIt, transferCount );

            SECTION("List&")
                coll.stealSectionAndInsertAt(insertPos, other, other.begin(), transferEndIt );

            SECTION("List&&")
                coll.stealSectionAndInsertAt(insertPos, std::move(other), other.begin(), transferEndIt  );

            SECTION("std::list&")
                coll.stealSectionAndInsertAt(insertPos, static_cast<std::list<typename CollType::Element>& >(other), other.begin(), transferEndIt  );

            SECTION("std::list&&")
                coll.stealSectionAndInsertAt(insertPos, std::move(static_cast<std::list<typename CollType::Element>& >(other)), other.begin(), transferEndIt  );
        }

        SECTION("full transfer")
        {
            transferBeginIndex = 0;

            typename CollType::ConstIterator transferEndIt = other.end();
            transferCount = other.size();

            SECTION("List&")
                coll.stealSectionAndInsertAt(insertPos, other, other.begin(), transferEndIt );

            SECTION("List&&")
                coll.stealSectionAndInsertAt(insertPos, std::move(other), other.begin(), transferEndIt  );

            SECTION("std::list&")
                coll.stealSectionAndInsertAt(insertPos, static_cast<std::list<typename CollType::Element>& >(other), other.begin(), transferEndIt  );

            SECTION("std::list&&")
                coll.stealSectionAndInsertAt(insertPos, std::move(static_cast<std::list<typename CollType::Element>& >(other)), other.begin(), transferEndIt  );
        }
    }
        
    if(otherInfo.size()>0)
    {
        SECTION("stealAndInsertAt")
        {
            transferCount = 1;

            SECTION("first")
            {            
                transferBeginIndex = 0;

                SECTION("List&")
                    coll.stealAndInsertAt(insertPos, other, other.begin() );

                SECTION("List&&")
                    coll.stealAndInsertAt(insertPos, std::move(other), other.begin()  );

                SECTION("std::list&")
                    coll.stealAndInsertAt(insertPos, static_cast<std::list<typename CollType::Element>& >(other), other.begin()  );

                SECTION("std::list&&")
                    coll.stealAndInsertAt(insertPos, std::move(static_cast<std::list<typename CollType::Element>& >(other)), other.begin()  );
            }

            SECTION("middle")
            {            
                transferBeginIndex = other.size()/2;

                auto transferIt = other.begin();
                std::advance(transferIt, transferBeginIndex);

                SECTION("List&")
                    coll.stealAndInsertAt(insertPos, other, transferIt );

                SECTION("List&&")
                    coll.stealAndInsertAt(insertPos, std::move(other), transferIt  );

                SECTION("std::list&")
                    coll.stealAndInsertAt(insertPos, static_cast<std::list<typename CollType::Element>& >(other), transferIt  );

                SECTION("std::list&&")
                    coll.stealAndInsertAt(insertPos, std::move(static_cast<std::list<typename CollType::Element>& >(other)), transferIt  );
            }

            SECTION("last")
            {            
                transferBeginIndex = other.size()-1;

                auto transferIt = other.end();
                --transferIt;

                SECTION("List&")
                    coll.stealAndInsertAt(insertPos, other, transferIt );

                SECTION("List&&")
                    coll.stealAndInsertAt(insertPos, std::move(other), transferIt  );

                SECTION("std::list&")
                    coll.stealAndInsertAt(insertPos, static_cast<std::list<typename CollType::Element>& >(other), transferIt  );

                SECTION("std::list&&")
                    coll.stealAndInsertAt(insertPos, std::move(static_cast<std::list<typename CollType::Element>& >(other)), transferIt  );
            }
        }
    }

    REQUIRE( other.getSize() == otherInfo.size()-transferCount );
    REQUIRE( coll.getSize() == info.size()+transferCount );
            
    typename CollType::Iterator insertEnd;
    if(infoInsertPos == info.end() )
    {
        // insert was at the end of the original collection => the insert end in
        // the resulting collection is its end
        insertEnd = coll.end();            
    }
    else
    {
        // insertPos now points to the element following the inserted range
        insertEnd = insertPos;
    }
    
    // insertPos now points to the end of the inserted range. We want it to point
    // to the first inserted element.
    insertPos = coll.begin();
    std::advance( insertPos, insertIndex);

    auto otherTransferBegin = other.begin();
    std::advance( otherTransferBegin, transferBeginIndex );
    
    auto otherInfoTransferBegin = otherInfo.begin();
    std::advance( otherInfoTransferBegin, transferBeginIndex );
    
    auto otherInfoTransferEnd = otherInfoTransferBegin;
    std::advance( otherInfoTransferEnd, transferCount );
                
    _verifyElementRange( coll.begin(), insertPos, info.begin(), infoInsertPos );
    _verifyElementRange( insertPos, insertEnd, otherInfoTransferBegin, otherInfoTransferEnd );
    _verifyElementRange( insertEnd, coll.end(), infoInsertPos, info.end() );

    _verifyElementRange( other.begin(), otherTransferBegin, otherInfo.begin(), otherInfoTransferBegin );

    // note that the transferred items are now gone from other. So otherTransferBegin marks is
    // the first element AFTER the removed section
    _verifyElementRange( otherTransferBegin, other.end(), otherInfoTransferEnd, otherInfo.end() );

}

template<typename CollType>
static void _verifyStealAndInsert( CollType& coll, CollType& other)
{
    SECTION("insert at begin")
        _verifyStealAndInsert( coll, other, coll.begin() );

    if( coll.getSize()>0 )
    {
        SECTION("insert in middle")
        {
            _verifyStealAndInsert( coll, other, ++coll.begin() );
        }
    }

    SECTION("insert at end")
        _verifyStealAndInsert( coll, other, coll.end() );
}

template<typename CollType>
static void _testStealAndInsert( CollType& coll, std::initializer_list< typename CollType::Element> newElList)
{
    SECTION("other empty")
    {
        CollType otherColl;

        _verifyStealAndInsert(coll, otherColl);
    }

    SECTION("other not empty")
    {
        CollType otherColl(newElList);

        _verifyStealAndInsert(coll, otherColl);
    }
}

template<typename CollType>
static void _testReverseOrder( CollType& coll )
{
    std::list< ElementInfo<typename CollType::Iterator> > origInfo = _getCollElementInfo(coll);

    auto initialFirstIt = coll.begin();
    auto initialEnd = coll.end();
    auto initialLastIt = coll.end();
    if(!coll.isEmpty())
        --initialLastIt;

    coll.reverseOrder();

    auto it = coll.begin();

    for( auto origInfoIt = origInfo.rbegin(); origInfoIt!=origInfo.rend(); ++origInfoIt)
    {
        REQUIRE( it != coll.end() );

        origInfoIt->verify(it);

        ++it;
    }

    REQUIRE( it==coll.end() );

    REQUIRE( coll.end() == initialEnd );

    if( !coll.isEmpty() )  
    {
        REQUIRE( coll.begin() == initialLastIt );
        REQUIRE( --coll.end() == initialFirstIt );
    }
}

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

        SECTION("stealAndInsert")
            _testStealAndInsert(coll, newElList);

        SECTION("reverseOrder")
            _testReverseOrder(coll);
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

        SECTION("stealAndInsert")
            _testStealAndInsert(coll, newElList);

        SECTION("reverseOrder")
            _testReverseOrder(coll);
        
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

        _testStealAllAndMergeSorted<int>( {17, 42, 17, 3}, {99, 6, 2, 102} );
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

            _testStealAllAndMergeSorted<TestCollectionElement_OrderedComparable_>(
                { TestCollectionElement_OrderedComparable_(17, 1),
                    TestCollectionElement_OrderedComparable_(42, 142),
                    TestCollectionElement_OrderedComparable_(17, 2),
                    TestCollectionElement_OrderedComparable_(3, 103),
                },
                { TestCollectionElement_OrderedComparable_(99, 1),
                    TestCollectionElement_OrderedComparable_(6, 142),
                    TestCollectionElement_OrderedComparable_(2, 2),
                    TestCollectionElement_OrderedComparable_(102, 103),
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

