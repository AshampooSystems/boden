#ifndef BDN_testCollection_H_
#define BDN_testCollection_H_

#include <bdn/test.h>

#include <vector>
#include <list>

namespace bdn
{
namespace test
{

class TestCollectionElement_UnorderedUncomparable_
{
public:
    TestCollectionElement_UnorderedUncomparable_()
    {
        _a = -1;
        _b = -1;
    }

    TestCollectionElement_UnorderedUncomparable_(int a, int b)
    {
        _a = a;
        _b = b;
    }

    TestCollectionElement_UnorderedUncomparable_(const TestCollectionElement_UnorderedUncomparable_& o)
    {
        _a = o._a;
        _b = o._b;
    }

    TestCollectionElement_UnorderedUncomparable_(TestCollectionElement_UnorderedUncomparable_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;
    }


    TestCollectionElement_UnorderedUncomparable_& operator=(const TestCollectionElement_UnorderedUncomparable_& o)
    {
        _a = o._a;
        _b = o._b;

        return *this;
    }

    TestCollectionElement_UnorderedUncomparable_& operator=(TestCollectionElement_UnorderedUncomparable_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;

        return *this;
    }

    int _a;
    int _b;
};




class TestCollectionElement_UnorderedComparable_ : public TestCollectionElement_UnorderedUncomparable_
{
public:
    TestCollectionElement_UnorderedComparable_()
    {
        _a = -1;
        _b = -1;
    }

    TestCollectionElement_UnorderedComparable_(int a, int b)
    {
        _a = a;
        _b = b;
    }

    TestCollectionElement_UnorderedComparable_(const TestCollectionElement_UnorderedComparable_& o)
    {
        _a = o._a;
        _b = o._b;
    }

    TestCollectionElement_UnorderedComparable_(TestCollectionElement_UnorderedComparable_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;
    }


    TestCollectionElement_UnorderedComparable_& operator=(const TestCollectionElement_UnorderedComparable_& o)
    {
        _a = o._a;
        _b = o._b;

        return *this;
    }

    TestCollectionElement_UnorderedComparable_& operator=(TestCollectionElement_UnorderedComparable_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;

        return *this;
    }

    bool operator==(const TestCollectionElement_UnorderedComparable_& o) const
    {
        return (_a == o._a
                && _b == o._b);
    }

    bool operator!=(const TestCollectionElement_UnorderedComparable_& o) const
    {
        return ! operator==(o);
    }

    int _a;
    int _b;
};

class TestCollectionElement_OrderedComparable_ : public TestCollectionElement_UnorderedComparable_
{
public:
    TestCollectionElement_OrderedComparable_()
    {
    }

    TestCollectionElement_OrderedComparable_(int a, int b)
        : TestCollectionElement_UnorderedComparable_(a, b)
    {
    }

    TestCollectionElement_OrderedComparable_(const TestCollectionElement_OrderedComparable_& o)
        : TestCollectionElement_UnorderedComparable_(o)
    {
    }

    TestCollectionElement_OrderedComparable_(TestCollectionElement_OrderedComparable_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;
    }


    TestCollectionElement_OrderedComparable_& operator=(const TestCollectionElement_OrderedComparable_& o)
    {
        _a = o._a;
        _b = o._b;

        return *this;
    }

    TestCollectionElement_OrderedComparable_& operator=(TestCollectionElement_OrderedComparable_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;

        return *this;
    }

    bool operator<(const TestCollectionElement_OrderedComparable_& o) const
    {
        // the ordering is only based on the "a" component, so that
        // we can have elements that are not equal, but have the same ordering position
        return (_a < o._a);
    }
};


inline bool _isCollectionElementEqual(const TestCollectionElement_UnorderedUncomparable_& l, const TestCollectionElement_UnorderedUncomparable_& r)
{
    return (l._a == r._a
            && l._b == r._b);
}

inline bool _isCollectionElementEqual(int l, int r)
{
    return (l==r);
}



template<class CollType>
inline void _verifyEmptyCollection(CollType& coll)
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
inline void _verifyEmptySequence(CollType& coll)
{
    _verifyEmptyCollection(coll);
    
    SECTION("getFirst")
        REQUIRE_THROWS_AS( coll.getFirst(), OutOfRangeError );

    SECTION("getLast")
        REQUIRE_THROWS_AS( coll.getLast(), OutOfRangeError );
}


template<class ElType, class ItType>
inline void _verifyIteratorIntervalContents(ItType it, ItType end, const std::list<ElType>& expectedElementList )
{
    for( ElType expectedEl: expectedElementList )
    {
        REQUIRE( it!=end );
        REQUIRE( _isCollectionElementEqual(*it, expectedEl ) );
        ++it;
    }

    REQUIRE( it==end );
}

template<class CollType>
inline void _verifyCollectionIteration(CollType& coll, const std::list<typename CollType::Element>& expectedElementList)
{
    SECTION("normal")
        _verifyIteratorIntervalContents<typename CollType::Element>( coll.begin(), coll.end(), expectedElementList );

    SECTION("const coll")
        _verifyIteratorIntervalContents<typename CollType::Element>( ((const CollType&)coll).begin(), ((const CollType&)coll).end(), expectedElementList );

    SECTION("constBegin/End")
        _verifyIteratorIntervalContents<typename CollType::Element>( coll.constBegin(), coll.constEnd(), expectedElementList );

    std::list< typename CollType::Element > reversedExpectedElementList( expectedElementList );
    std::reverse( reversedExpectedElementList.begin(), reversedExpectedElementList.end() );
        
    SECTION("reverse normal")
        _verifyIteratorIntervalContents<typename CollType::Element>( coll.reverseBegin(), coll.reverseEnd(), reversedExpectedElementList );

    SECTION("reverse const coll")
        _verifyIteratorIntervalContents<typename CollType::Element>( ((const CollType&)coll).reverseBegin(), ((const CollType&)coll).reverseEnd(), reversedExpectedElementList  );

    SECTION("constReverseBegin/End")
        _verifyIteratorIntervalContents<typename CollType::Element>( coll.constReverseBegin(), coll.constReverseEnd(), reversedExpectedElementList );
}


template<class CollType>
inline void _verifySequenceCollectionReadOnly(CollType& coll, std::list<typename CollType::Element> expectedElementList )
{
    SECTION("size")
    {
        REQUIRE( coll.size() == expectedElementList.size() );
        REQUIRE( coll.getSize() == expectedElementList.size() );
    }
    
    SECTION("maxSize")
    {
        // max size should be "reasonably large". It might depend on the size of the elements, though,
        // So this test might not work for all element types. But it works for the types we use in
        // our test.
        REQUIRE( coll.max_size() >= 0x00ffffff );
        REQUIRE( coll.getMaxSize() >= 0x00ffffff );
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
            REQUIRE( _isCollectionElementEqual(coll.getFirst(), expectedElementList.front() ) );
    }
    
    SECTION("getLast")
    {
        if(expectedElementList.size() == 0)
            REQUIRE_THROWS_AS( coll.getLast(), OutOfRangeError );
        else
            REQUIRE( _isCollectionElementEqual( coll.getLast(), expectedElementList.back() ) );
    }
    
    SECTION("iteration")
        _verifyCollectionIteration( coll, expectedElementList );
}



template<class CollType, typename... ConstructArgs>
inline void _verifyCollectionInsertAt(
    CollType& coll,
    int insertIndex,
    std::list<typename CollType::Element> expectedElementList,
    std::initializer_list<typename CollType::Element> newElList,
    std::function< bool(const typename CollType::Element&) > isMovedRemnant,
    typename CollType::Element expectedConstructedEl,
    ConstructArgs... constructArgs)
{
    std::list<typename CollType::Element> newExpectedElementList = expectedElementList;

    typename CollType::Iterator insertIt = coll.begin();
    typename std::list<typename CollType::Element>::iterator expectedInsertIt = newExpectedElementList.begin();
    for(int i=0; i<insertIndex; i++)
    {
        ++insertIt;
        ++expectedInsertIt;
    }

    typename CollType::Element elToAdd = *newElList.begin();

    SECTION("ref")
    {
        coll.insertAt( insertIt, elToAdd );
        newExpectedElementList.insert( expectedInsertIt, elToAdd );

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
    }

    SECTION("move")
    {
        newExpectedElementList.insert( expectedInsertIt, elToAdd );
        coll.insertAt( insertIt, std::move(elToAdd) );        

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );

        // elToAdd should not have the same value anymore
        REQUIRE( isMovedRemnant(elToAdd) );
    }

    SECTION("sequence")
    {
        SECTION("iterators")
        {
            SECTION("empty")
            {
                coll.insertSequenceAt( insertIt, newElList.begin(), newElList.begin() );
                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {
                coll.insertSequenceAt( insertIt, newElList.begin(), newElList.end() );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll.insertSequenceAt( insertIt, {} );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {            
                coll.insertSequenceAt( insertIt, newElList );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }
        }
    }

    SECTION("multiple copies")
    {
        SECTION("0 times")
        {
            coll.insertMultipleCopiesAt( insertIt, 0, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }

        SECTION("1 times")
        {
            coll.insertMultipleCopiesAt( insertIt, 1, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }

        SECTION("3 times")
        {
            coll.insertMultipleCopiesAt( insertIt, 3, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, 3, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }
    }

    SECTION("new")
    {
        coll. template insertNewAt<ConstructArgs...>( insertIt, std::forward<ConstructArgs>(constructArgs)... );
        newExpectedElementList.insert( expectedInsertIt, expectedConstructedEl );

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
    }
}



template<class CollType, typename... ConstructArgs>
inline void _verifyCollectionAdd(
    CollType& coll,
    std::list<typename CollType::Element> expectedElementList,
    std::initializer_list<typename CollType::Element> newElList,
    std::function< bool(const typename CollType::Element&) > isMovedRemnant,
    typename CollType::Element expectedConstructedEl,
    ConstructArgs... constructArgs
)
{
    std::list<typename CollType::Element> newExpectedElementList = expectedElementList;

    typename std::list<typename CollType::Element>::iterator expectedInsertIt = newExpectedElementList.end();
    
    typename CollType::Element elToAdd = *newElList.begin();

    SECTION("ref")
    {
        coll.add( elToAdd );
        newExpectedElementList.insert( expectedInsertIt, elToAdd );

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
    }

    SECTION("move")
    {
        newExpectedElementList.insert( expectedInsertIt, elToAdd );
        coll.add( std::move(elToAdd) );        

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );

        // elToAdd should not have the same value anymore
        REQUIRE( isMovedRemnant(elToAdd) );
    }

    SECTION("sequence")
    {
        SECTION("iterators")
        {
            SECTION("empty")
            {
                coll.addSequence( newElList.begin(), newElList.begin() );
                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {
                coll.addSequence( newElList.begin(), newElList.end() );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll.addSequence( {} );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {            
                coll.addSequence( newElList );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }
        }
    }

    SECTION("multiple copies")
    {
        SECTION("0 times")
        {
            coll.addMultipleCopies( 0, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }

        SECTION("1 times")
        {
            coll.addMultipleCopies( 1, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }

        SECTION("3 times")
        {
            coll.addMultipleCopies( 3, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, 3, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }
    }

    SECTION("new")
    {
        coll.addNew( std::forward<ConstructArgs>(constructArgs)... );
        newExpectedElementList.insert( expectedInsertIt, expectedConstructedEl );

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
    }
}



template<class CollType, typename... ConstructArgs>
inline void _verifyCollectionInsertAtBegin(
    CollType& coll,
    std::list<typename CollType::Element> expectedElementList,
    std::initializer_list<typename CollType::Element> newElList,
    std::function< bool(const typename CollType::Element&) > isMovedRemnant,
    typename CollType::Element expectedConstructedEl,
    ConstructArgs... constructArgs
)
{
    std::list<typename CollType::Element> newExpectedElementList = expectedElementList;

    typename std::list<typename CollType::Element>::iterator expectedInsertIt = newExpectedElementList.begin();
    
    typename CollType::Element elToAdd = *newElList.begin();

    SECTION("ref")
    {
        coll.insertAtBegin( elToAdd );
        newExpectedElementList.insert( expectedInsertIt, elToAdd );

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
    }

    SECTION("move")
    {
        newExpectedElementList.insert( expectedInsertIt, elToAdd );
        coll.insertAtBegin( std::move(elToAdd) );        

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );

        // elToAdd should not have the same value anymore
        REQUIRE( isMovedRemnant(elToAdd) );
    }

    SECTION("sequence")
    {
        SECTION("iterators")
        {
            SECTION("empty")
            {
                coll.insertSequenceAtBegin( newElList.begin(), newElList.begin() );
                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {
                coll.insertSequenceAtBegin( newElList.begin(), newElList.end() );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll.insertSequenceAtBegin( {} );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {            
                coll.insertSequenceAtBegin( newElList );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
            }
        }
    }

    SECTION("multiple copies")
    {
        SECTION("0 times")
        {
            coll.insertMultipleCopiesAtBegin( 0, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }

        SECTION("1 times")
        {
            coll.insertMultipleCopiesAtBegin( 1, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }

        SECTION("3 times")
        {
            coll.insertMultipleCopiesAtBegin( 3, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, 3, *newElList.begin() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
        }
    }

    SECTION("new")
    {
        coll.insertNewAtBegin( std::forward<ConstructArgs>(constructArgs)... );
        newExpectedElementList.insert( expectedInsertIt, expectedConstructedEl );

        _verifySequenceCollectionReadOnly( coll, newExpectedElementList );
    }
}


    
template<class CollType, typename... ConstructArgs>
inline void _verifySequenceCollection(
    CollType& coll,
    std::list<typename CollType::Element> expectedElementList,
    std::initializer_list<typename CollType::Element> newElList,
    std::function< bool(const typename CollType::Element&) > isMovedRemnant,
    typename CollType::Element expectedConstructedEl,
    ConstructArgs... constructArgs
    )
{
    _verifySequenceCollectionReadOnly(coll, expectedElementList);

    std::list<typename CollType::Element> newExpectedElementList( expectedElementList );

    SECTION("clear")
    {
        coll.clear();

        _verifySequenceCollectionReadOnly( coll, {});
    }

    SECTION("operator=")
    {
        SECTION("other collection")
        {
            SECTION("empty")
            {
                CollType otherColl;

                SECTION("ref")
                {
                    coll = otherColl;
                    _verifySequenceCollectionReadOnly( coll, {});
                }

                SECTION("move")
                {
                    coll = std::move(otherColl);
                    _verifySequenceCollectionReadOnly( coll, {});

                    REQUIRE( otherColl.size() == 0);
                }
            }

            SECTION("non-empty")
            {
                CollType otherColl;
                otherColl.add( expectedConstructedEl );

                SECTION("ref")
                {
                    coll = otherColl;
                    _verifySequenceCollectionReadOnly( coll, {expectedConstructedEl} );
                }

                SECTION("move")
                {
                    coll = std::move(otherColl);
                    _verifySequenceCollectionReadOnly( coll, {expectedConstructedEl} );

                    REQUIRE( otherColl.size() == 0);
                }
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll = {};
                _verifySequenceCollectionReadOnly( coll, {});
            }

            SECTION("non-empty")
            {
                coll = {expectedConstructedEl};

                _verifySequenceCollectionReadOnly( coll, {expectedConstructedEl} );
            }
        }
    }
    
    if( expectedElementList.size() > 0 )
    {
        SECTION("removeAt")
        {
            SECTION("first")
            {
                coll.removeAt( coll.begin() );
                newExpectedElementList.erase( newExpectedElementList.begin() );                   
                    
                _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
            }

            SECTION("last")
            {
                coll.removeAt( --coll.end() );
                newExpectedElementList.erase( --newExpectedElementList.end() );                   
                    
                _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
            }

            if( expectedElementList.size() > 1)
            {
                SECTION("mid")
                {
                    coll.removeAt( ++coll.begin() );
                    newExpectedElementList.erase( ++newExpectedElementList.begin() );                   
                    
                    _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
                }
            }
        }

        SECTION("removeFirst")
        {
            coll.removeFirst();
            newExpectedElementList.erase( newExpectedElementList.begin() );                   
        }

        SECTION("removeLast")
        {
            coll.removeLast();
            newExpectedElementList.erase( --newExpectedElementList.end() );                   
                    
            _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
        }
    }

    SECTION("insertAt")
    {
        SECTION("at begin")
            _verifyCollectionInsertAt(coll, 0, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );

        if(expectedElementList.size()>1)
        {
            SECTION("in middle")
                _verifyCollectionInsertAt(coll, 1, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
        }

        SECTION("at end")
            _verifyCollectionInsertAt(coll, expectedElementList.size(), expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
    }

    SECTION("add")
    {
        _verifyCollectionAdd(coll, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
    }

    SECTION("insertAtBegin")
    {
        _verifyCollectionInsertAtBegin(coll, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
    }

    SECTION("setSize")
    {
        SECTION("to 0")
        {
            coll.setSize(0);
            newExpectedElementList.clear();

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
        }

        if(expectedElementList.size()>0)
        {
            SECTION("to one less")
            {
                coll.setSize( coll.size()-1 );
                newExpectedElementList.erase( --newExpectedElementList.end() );

                _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
            }
        }

        SECTION("to one more")
        {
            coll.setSize( coll.size()+1 );
            newExpectedElementList.push_back( typename CollType::Element() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
        }

        SECTION("to 100 more")
        {
            coll.setSize( coll.size()+100 );

            for(int i=0; i<100; i++)
                newExpectedElementList.push_back( typename CollType::Element() );

            _verifySequenceCollectionReadOnly( coll, newExpectedElementList);
        }
    }
}

template<class CollType, class ItType >
inline void _verifyIndexedCollectionAccess(CollType& coll, int index, ItType expectedElementIt )
{
    if(index!=coll.size() )
    {
        SECTION("atIndex")
            REQUIRE( &coll.atIndex(index) == &*expectedElementIt );

        SECTION("operator[]")
            REQUIRE( &coll[index] == &*expectedElementIt );
    }

    SECTION("indexToIterator")
        REQUIRE( coll.indexToIterator(index) == expectedElementIt );

    SECTION("iteratorToIndex")
        REQUIRE( coll.iteratorToIndex(expectedElementIt) == index );
}

template<class CollType>
inline void _testCollectionIndexedAccess(CollType& coll)
{
    SECTION("0")
        _verifyIndexedCollectionAccess( coll, 0, coll.begin() );
            
    SECTION("1")
        _verifyIndexedCollectionAccess( coll, 1, ++coll.begin() );

    SECTION("last")
        _verifyIndexedCollectionAccess( coll, coll.size()-1, --coll.end() );

    SECTION("end")
        _verifyIndexedCollectionAccess( coll, coll.size(), coll.end() );
}

template<class CollType>
inline void _testGenericCollectionPrepareForSize(CollType& coll)
{
    std::list< typename CollType::Element > origElements( coll.begin(), coll.end() );

    typename CollType::Size prepareFor=0;
    
    SECTION("0")
        prepareFor = 0;

    SECTION("1")
        prepareFor = 1;

    SECTION("size")
        prepareFor = coll.size();

    SECTION("size+1")
        prepareFor = coll.size()+1;

    SECTION("size+1000")
        prepareFor = coll.size()+1000;
    
    coll.prepareForSize( prepareFor );

    // prepareForSize should NEVER change the collection contents
    _verifySequenceCollectionReadOnly( coll, origElements );
}

template<class CollType, class ItType>
inline void _verifyCollectionFindVariant(CollType& coll, typename CollType::Element toFind, ItType expectedResult )
{
    SECTION("no startPos")
    {
        ItType it = coll.find( toFind );
        REQUIRE( it == expectedResult );
    }

    SECTION("with startPos")
    {
        SECTION("begin")
        {
            ItType it = coll.find( toFind, coll.begin() );
            REQUIRE( it == expectedResult );
        }

        SECTION("expected find position")
        {
            ItType it = coll.find( toFind, expectedResult );
            REQUIRE( it == expectedResult );
        }

        if( expectedResult!=coll.end())
        {
            SECTION("one after expected find position")
            {
                ItType startPos = expectedResult;
                ++startPos;

                ItType it = coll.find( toFind, startPos );
                REQUIRE( it != expectedResult );
            }
        }

        SECTION("end")
        {
            ItType it = coll.find( toFind, coll.end() );
            REQUIRE( it == coll.end() );
        }
    }
}


template<class CollType>
inline void _verifyCollectionFind(CollType& coll, typename CollType::Element toFind, typename CollType::Iterator expectedResult )
{
    SECTION("normal")
        _verifyCollectionFindVariant<CollType, typename CollType::Iterator>( coll, toFind, expectedResult);
    
    SECTION("const")
    {
        const CollType& constColl(coll);

        // convert the const iterator to non-const
        typename CollType::ConstIterator constExpectedResult = constColl.begin();
        std::advance( constExpectedResult, std::distance(coll.begin(), expectedResult ) );

        _verifyCollectionFindVariant<const CollType, typename CollType::ConstIterator>( constColl, toFind, constExpectedResult);
    }
}




template<class CollType, class ItType>
inline void verifyReverseFindForSpecificVariant(CollType& coll, typename CollType::Element toFind, ItType expectedResult, ItType posAfterExpectedPos )
{
    SECTION("no startPos")
    {
        ItType it = coll.reverseFind( toFind );
        REQUIRE( it == expectedResult );
    }

    SECTION("with startPos")
    {
        SECTION("begin")
        {
            ItType it = coll.reverseFind( toFind, coll.begin() );

            if( !coll.isEmpty() && coll.getFirst() == toFind )
                REQUIRE( it == coll.begin() );
            else
                REQUIRE( it == coll.end() );
        }

        SECTION("expected find position")
        {
            ItType it = coll.reverseFind( toFind, expectedResult );
            REQUIRE( it == expectedResult );
        }

        if( expectedResult!=coll.begin() )
        {
            SECTION("after expected find position")
            {
                ItType it = coll.reverseFind( toFind, posAfterExpectedPos );

                if(expectedResult==coll.end())
                    REQUIRE( it == expectedResult );
                else
                    REQUIRE( it != expectedResult );
            }
        }

        if(!coll.isEmpty())
        {
            SECTION("last")
            {
                ItType it = coll.reverseFind( toFind, --coll.end() );
                REQUIRE( it == expectedResult );
            }
        }

        SECTION("end")
        {
            ItType it = coll.reverseFind( toFind, coll.end() );
            REQUIRE( it == expectedResult );
        }
    }
}

template<class CollType>
inline void verifyReverseFind(CollType& coll, typename CollType::Element toFind, typename CollType::Iterator expectedResult )
{
    SECTION("normal")
    {
        typename CollType::Iterator posAfterExpectedPos = expectedResult;
        if(expectedResult!=coll.begin() )
            --posAfterExpectedPos;

        verifyReverseFindForSpecificVariant<CollType, typename CollType::Iterator>( coll, toFind, expectedResult, posAfterExpectedPos);
    }
    
    SECTION("const")
    {
        const CollType& constColl(coll);

        // convert the const iterator to non-const
        typename CollType::ConstIterator constExpectedResult = constColl.begin();
        std::advance( constExpectedResult, std::distance(coll.begin(), expectedResult) );

        typename CollType::ConstIterator posAfterExpectedPos = constExpectedResult;
        if(constExpectedResult!=constColl.begin())
            --posAfterExpectedPos;

        verifyReverseFindForSpecificVariant<const CollType, typename CollType::ConstIterator>( constColl, toFind, constExpectedResult, posAfterExpectedPos);
    }
}

template<class CollType>
inline void verifyFindCondition(CollType& coll, std::function< bool(const typename CollType::Element&) > conditionFunc, typename CollType::Iterator expectedResult )
{
    SECTION("normal")
    {
        typename CollType::Iterator it = coll.findCondition( conditionFunc );
        REQUIRE( it == expectedResult );
    }

    SECTION("const")
    {
        const CollType& constColl(coll);

        typename CollType::ConstIterator it = constColl.findCondition( conditionFunc );

        if(expectedResult==coll.end())
            REQUIRE( it == constColl.end() );
        else
        {
            REQUIRE( &*it == &*expectedResult );
        }
    }
}


template<class CollType>
inline void verifyReverseFindCondition(CollType& coll, std::function< bool(const typename CollType::Element&) > conditionFunc, typename CollType::Iterator expectedResult )
{
    SECTION("normal")
    {
        typename CollType::Iterator it = coll.reverseFindCondition( conditionFunc );
        REQUIRE( it == expectedResult );
    }

    SECTION("const")
    {
        const CollType& constColl(coll);

        typename CollType::ConstIterator it = constColl.reverseFindCondition( conditionFunc );

        if(expectedResult==coll.end())
            REQUIRE( it == constColl.end() );
        else
        {
            REQUIRE( &*it == &*expectedResult );
        }
    }
}

template<class CollType>
inline void _testCollectionFind(std::initializer_list<typename CollType::Element> elements, const typename CollType::Element& elNotInColl )
{
    SECTION("find")
    {
        CollType coll;

        SECTION("empty")
        {
            SECTION("not found")
                _verifyCollectionFind( coll, elNotInColl, coll.end() );
        }

        SECTION("not empty")
        {        
            coll.addSequence(elements);

            // the first element must equal the third one for these tests to work
            REQUIRE( coll.getFirst() == *(++(++coll.begin())) );

            SECTION("first")
                _verifyCollectionFind( coll, coll.getFirst(), coll.begin() );

            SECTION("last")
                _verifyCollectionFind( coll, coll.getLast(), --coll.end() );

            SECTION("second")
                _verifyCollectionFind( coll, *++coll.begin(), ++coll.begin() );

            SECTION("not found")
                _verifyCollectionFind( coll, elNotInColl, coll.end() );
        }
    }

    SECTION("findCondition")
    {
        CollType coll;

        SECTION("empty")
        {
            SECTION("not found")
            {
                verifyFindCondition(
                    coll,
                    [](const typename CollType::Element& el)
                    {
                        return false;
                    },
                    coll.end() );
            }
        }

        SECTION("not empty")
        {        
            coll.addSequence(elements);

            // the first element must equal the third one for these tests to work
            REQUIRE( coll.getFirst() == *(++(++coll.begin())) );

            SECTION("first")
            {
                typename CollType::Element toFind = coll.getFirst();

                verifyFindCondition(
                    coll,
                    [toFind](const typename CollType::Element& el)
                    {
                        return (el == toFind);
                    },
                    coll.begin() );
            }

            SECTION("last")
            {
                typename CollType::Element toFind = coll.getLast();

                verifyFindCondition(
                    coll,
                    [toFind](const typename CollType::Element& el)
                    {
                        return (el == toFind);
                    },
                    --coll.end() );
            }

            SECTION("second")
            {
                typename CollType::Element toFind = *++coll.begin();

                verifyFindCondition(
                    coll,
                    [toFind](const typename CollType::Element& el)
                    {
                        return (el == toFind);
                    },
                    ++coll.begin() );
            }

            SECTION("not found")
            {
                verifyFindCondition(
                    coll,
                    [](const typename CollType::Element& el)
                    {
                        return false;
                    },
                    coll.end() );
            }
        }
    }



    SECTION("reverseFind")
    {
        CollType coll;

        SECTION("empty")
        {
            SECTION("not found")
                verifyReverseFind( coll, elNotInColl, coll.end() );
        }

        SECTION("not empty")
        {        
            coll.addSequence(elements);

            // the first element must equal the third one for these tests to work
            REQUIRE( coll.getFirst() == *(++(++coll.begin())) );

            SECTION("first")
            {
                // the first and the third element compare equal. So we expect to find the third one
                verifyReverseFind( coll, coll.getFirst(), ++(++coll.begin()) );
            }

            SECTION("last")
                verifyReverseFind( coll, coll.getLast(), --coll.end() );

            SECTION("second")
                verifyReverseFind( coll, *++coll.begin(), ++coll.begin() );

            SECTION("not found")
                verifyReverseFind( coll, elNotInColl, coll.end() );
        }
    }

    SECTION("reverseFindCondition")
    {
        CollType coll;

        SECTION("empty")
        {
            SECTION("not found")
            {
                verifyReverseFindCondition(
                    coll,
                    [](const typename CollType::Element& el)
                    {
                        return false;
                    },
                    coll.end() );
            }
        }

        SECTION("not empty")
        {        
            coll.addSequence(elements);

            // the first element must equal the third one for these tests to work
            REQUIRE( coll.getFirst() == *(++(++coll.begin())) );

            SECTION("first")
            {
                typename CollType::Element toFind = coll.getFirst();

                verifyReverseFindCondition(
                    coll,
                    [toFind](const typename CollType::Element& el)
                    {
                        return (el == toFind);
                    },
                    // the first element in the sequence is also the third one. So we should find that
                    ++(++coll.begin()) );
            }

            SECTION("last")
            {
                typename CollType::Element toFind = coll.getLast();

                verifyReverseFindCondition(
                    coll,
                    [toFind](const typename CollType::Element& el)
                    {
                        return (el == toFind);
                    },
                    --coll.end() );
            }

            SECTION("second")
            {
                typename CollType::Element toFind = *++coll.begin();

                verifyReverseFindCondition(
                    coll,
                    [toFind](const typename CollType::Element& el)
                    {
                        return (el == toFind);
                    },
                    ++coll.begin() );
            }

            SECTION("not found")
            {
                verifyReverseFindCondition(
                    coll,
                    [](const typename CollType::Element& el)
                    {
                        return false;
                    },
                    coll.end() );
            }
        }
    }
 
}

template<class CollType>
inline void _verifyCollectionSortResult( CollType& coll, std::initializer_list<typename CollType::Element> elements, bool expectInvertedOrder = false )
{
    // verify that the elements are all in the correct order
    auto it = coll.begin();
    while(it != coll.end() )
    {
        auto nextIt = it;
        ++nextIt;

        if(nextIt == coll.end() )
            break;

        if( expectInvertedOrder )
            REQUIRE( ! (*it < *nextIt) );
        else
            REQUIRE( ! (*nextIt < *it) );

        ++it;
    }

    // verify that all elements are still in the collection
    REQUIRE( coll.size() == elements.size() );

    for( auto& el: elements)
        REQUIRE( coll.find(el) != coll.end() );
}

template<class CollType>
inline void _testCollectionSort(
    std::initializer_list<typename CollType::Element> elements,
    std::initializer_list<typename CollType::Element> stableSortedElements )
{
    SECTION("empty")
    {
        CollType coll;

        SECTION("sort")
        {
            coll.sort();
            _verifySequenceCollectionReadOnly( coll, {} );
        }

        SECTION("sort(ascending)")
        {
            coll.sort( ascending<typename CollType::Element> );
            _verifySequenceCollectionReadOnly( coll, {} );
        }

        SECTION("sort(descending)")
        {
            coll.sort( descending<typename CollType::Element> );
            _verifySequenceCollectionReadOnly( coll, {} );
        }

        SECTION("sort with compare func")
        {
            coll.sort(
                [](typename CollType::Element& a, typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            _verifySequenceCollectionReadOnly( coll, {} );
        }

        SECTION("stableSort")
        {
            coll.stableSort();
            _verifySequenceCollectionReadOnly( coll, {} );
        }

        SECTION("stableSort(ascending)")
        {
            coll.stableSort( ascending<typename CollType::Element> );
            _verifySequenceCollectionReadOnly( coll, {} );
        }

        SECTION("stableSort(descending)")
        {
            coll.stableSort( descending<typename CollType::Element> );
            _verifySequenceCollectionReadOnly( coll, {} );
        }

        SECTION("stableSort with compare func")
        {
            coll.stableSort(
                [](const typename CollType::Element& a, const typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            _verifySequenceCollectionReadOnly( coll, {} );
        }
    }

    SECTION("not empty")
    {
        CollType coll( elements );

        SECTION("sort")
        {
            coll.sort();

            _verifyCollectionSortResult( coll, elements );
        }

        SECTION("sort(ascending)")
        {
            coll.sort( ascending<typename CollType::Element> );

            _verifyCollectionSortResult( coll, elements );
        }

        SECTION("sort(descending)")
        {
            coll.sort( descending<typename CollType::Element> );

            _verifyCollectionSortResult( coll, elements, true );
        }

        SECTION("sort with compare func")
        {
            coll.sort(
                [](typename CollType::Element& a, typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            _verifyCollectionSortResult( coll, elements, true );
        }

        SECTION("stableSort")
        {
            coll.stableSort();

            _verifyCollectionSortResult( coll, elements );

            _verifySequenceCollectionReadOnly( coll, stableSortedElements );
        }

        SECTION("stableSort(ascending)")
        {
            coll.stableSort( ascending<typename CollType::Element> );

            _verifyCollectionSortResult( coll, elements );
        }

        SECTION("stableSort(descending)")
        {
            coll.stableSort( descending<typename CollType::Element> );

            _verifyCollectionSortResult( coll, elements, true );
        }

        SECTION("stableSort with compare func")
        {
            coll.stableSort(
                [](const typename CollType::Element& a, const typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            _verifyCollectionSortResult( coll, elements, true );

            std::list<typename CollType::Element> expectedSortedElements( stableSortedElements.begin(), stableSortedElements.end() );
            expectedSortedElements.reverse();

            _verifySequenceCollectionReadOnly( coll, expectedSortedElements );
        }
    }
}


}
}


#endif