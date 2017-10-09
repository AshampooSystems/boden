#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Array.h>

using namespace bdn;


class TestArrayElement_
{
public:
    TestArrayElement_()
    {
        _a = -1;
        _b = -1;
    }

    TestArrayElement_(int a, int b)
    {
        _a = a;
        _b = b;
    }

    TestArrayElement_(const TestArrayElement_& o)
    {
        _a = o._a;
        _b = o._b;
    }

    TestArrayElement_(TestArrayElement_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;
    }


    TestArrayElement_& operator=(const TestArrayElement_& o)
    {
        _a = o._a;
        _b = o._b;

        return *this;
    }

    TestArrayElement_& operator=(TestArrayElement_&& o)
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




class TestUnorderedComparableArrayElement_ : public TestArrayElement_
{
public:
    TestUnorderedComparableArrayElement_()
    {
        _a = -1;
        _b = -1;
    }

    TestUnorderedComparableArrayElement_(int a, int b)
    {
        _a = a;
        _b = b;
    }

    TestUnorderedComparableArrayElement_(const TestUnorderedComparableArrayElement_& o)
    {
        _a = o._a;
        _b = o._b;
    }

    TestUnorderedComparableArrayElement_(TestUnorderedComparableArrayElement_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;
    }


    TestUnorderedComparableArrayElement_& operator=(const TestUnorderedComparableArrayElement_& o)
    {
        _a = o._a;
        _b = o._b;

        return *this;
    }

    TestUnorderedComparableArrayElement_& operator=(TestUnorderedComparableArrayElement_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;

        return *this;
    }

    bool operator==(const TestUnorderedComparableArrayElement_& o) const
    {
        return (_a == o._a
                && _b == o._b);
    }

    bool operator!=(const TestUnorderedComparableArrayElement_& o) const
    {
        return ! operator==(o);
    }

    int _a;
    int _b;
};

class TestOrderedArrayElement_ : public TestUnorderedComparableArrayElement_
{
public:
    TestOrderedArrayElement_()
    {
    }

    TestOrderedArrayElement_(int a, int b)
        : TestUnorderedComparableArrayElement_(a, b)
    {
    }

    TestOrderedArrayElement_(const TestOrderedArrayElement_& o)
        : TestUnorderedComparableArrayElement_(o)
    {
    }

    TestOrderedArrayElement_(TestOrderedArrayElement_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;
    }


    TestOrderedArrayElement_& operator=(const TestOrderedArrayElement_& o)
    {
        _a = o._a;
        _b = o._b;

        return *this;
    }

    TestOrderedArrayElement_& operator=(TestOrderedArrayElement_&& o)
    {
        _a = o._a;
        _b = o._b;

        o._a = -2;
        o._b = -2;

        return *this;
    }

    bool operator<(const TestOrderedArrayElement_& o) const
    {
        // the ordering is only based on the "a" component, so that
        // we can have elements that are not equal, but have the same ordering position
        return (_a < o._a);
    }
};


static bool _isCollElementEqual(const TestArrayElement_& l, const TestArrayElement_& r)
{
    return (l._a == r._a
            && l._b == r._b);
}

static bool _isCollElementEqual(int l, int r)
{
    return (l==r);
}



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
        REQUIRE( _isCollElementEqual(*it, expectedEl ) );
        ++it;
    }

    REQUIRE( it==end );
}

template<class CollType>
static void verifyCollIteration(CollType& coll, const std::list<typename CollType::Element>& expectedElementList)
{
    SECTION("normal")
        verifyIterators<typename CollType::Element>( coll.begin(), coll.end(), expectedElementList );

    SECTION("const coll")
        verifyIterators<typename CollType::Element>( ((const CollType&)coll).begin(), ((const CollType&)coll).end(), expectedElementList );

    SECTION("constBegin/End")
        verifyIterators<typename CollType::Element>( coll.constBegin(), coll.constEnd(), expectedElementList );

    std::list< typename CollType::Element > reversedExpectedElementList( expectedElementList );
    std::reverse( reversedExpectedElementList.begin(), reversedExpectedElementList.end() );
        
    SECTION("reverse normal")
        verifyIterators<typename CollType::Element>( coll.reverseBegin(), coll.reverseEnd(), reversedExpectedElementList );

    SECTION("reverse const coll")
        verifyIterators<typename CollType::Element>( ((const CollType&)coll).reverseBegin(), ((const CollType&)coll).reverseEnd(), reversedExpectedElementList  );

    SECTION("constReverseBegin/End")
        verifyIterators<typename CollType::Element>( coll.constReverseBegin(), coll.constReverseEnd(), reversedExpectedElementList );
}


template<class CollType>
static void verifyReadOnlySequence(CollType& coll, std::list<typename CollType::Element> expectedElementList )
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
            REQUIRE( _isCollElementEqual(coll.getFirst(), expectedElementList.front() ) );
    }
    
    SECTION("getLast")
    {
        if(expectedElementList.size() == 0)
            REQUIRE_THROWS_AS( coll.getLast(), OutOfRangeError );
        else
            REQUIRE( _isCollElementEqual( coll.getLast(), expectedElementList.back() ) );
    }
    
    SECTION("iteration")
    verifyCollIteration( coll, expectedElementList );
}



template<class CollType, typename... ConstructArgs>
static void verifyInsertAt(
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

        verifyReadOnlySequence( coll, newExpectedElementList );
    }

    SECTION("move")
    {
        newExpectedElementList.insert( expectedInsertIt, elToAdd );
        coll.insertAt( insertIt, std::move(elToAdd) );        

        verifyReadOnlySequence( coll, newExpectedElementList );

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
                verifyReadOnlySequence( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {
                coll.insertSequenceAt( insertIt, newElList.begin(), newElList.end() );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll.insertSequenceAt( insertIt, {} );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {            
                coll.insertSequenceAt( insertIt, newElList );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }
        }
    }

    SECTION("multiple copies")
    {
        SECTION("0 times")
        {
            coll.insertMultipleCopiesAt( insertIt, 0, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }

        SECTION("1 times")
        {
            coll.insertMultipleCopiesAt( insertIt, 1, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }

        SECTION("3 times")
        {
            coll.insertMultipleCopiesAt( insertIt, 3, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, 3, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }
    }

    SECTION("new")
    {
        coll. template insertNewAt<ConstructArgs...>( insertIt, std::forward<ConstructArgs>(constructArgs)... );
        newExpectedElementList.insert( expectedInsertIt, expectedConstructedEl );

        verifyReadOnlySequence( coll, newExpectedElementList );
    }
}



template<class CollType, typename... ConstructArgs>
static void verifyAdd(
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

        verifyReadOnlySequence( coll, newExpectedElementList );
    }

    SECTION("move")
    {
        newExpectedElementList.insert( expectedInsertIt, elToAdd );
        coll.add( std::move(elToAdd) );        

        verifyReadOnlySequence( coll, newExpectedElementList );

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
                verifyReadOnlySequence( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {
                coll.addSequence( newElList.begin(), newElList.end() );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll.addSequence( {} );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {            
                coll.addSequence( newElList );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }
        }
    }

    SECTION("multiple copies")
    {
        SECTION("0 times")
        {
            coll.addMultipleCopies( 0, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }

        SECTION("1 times")
        {
            coll.addMultipleCopies( 1, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }

        SECTION("3 times")
        {
            coll.addMultipleCopies( 3, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, 3, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }
    }

    SECTION("new")
    {
        coll.addNew( std::forward<ConstructArgs>(constructArgs)... );
        newExpectedElementList.insert( expectedInsertIt, expectedConstructedEl );

        verifyReadOnlySequence( coll, newExpectedElementList );
    }
}



template<class CollType, typename... ConstructArgs>
static void verifyInsertAtBegin(
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

        verifyReadOnlySequence( coll, newExpectedElementList );
    }

    SECTION("move")
    {
        newExpectedElementList.insert( expectedInsertIt, elToAdd );
        coll.insertAtBegin( std::move(elToAdd) );        

        verifyReadOnlySequence( coll, newExpectedElementList );

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
                verifyReadOnlySequence( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {
                coll.insertSequenceAtBegin( newElList.begin(), newElList.end() );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll.insertSequenceAtBegin( {} );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }

            SECTION("non-empty")
            {            
                coll.insertSequenceAtBegin( newElList );
                newExpectedElementList.insert( expectedInsertIt, newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, newExpectedElementList );
            }
        }
    }

    SECTION("multiple copies")
    {
        SECTION("0 times")
        {
            coll.insertMultipleCopiesAtBegin( 0, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }

        SECTION("1 times")
        {
            coll.insertMultipleCopiesAtBegin( 1, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }

        SECTION("3 times")
        {
            coll.insertMultipleCopiesAtBegin( 3, *newElList.begin() );
            newExpectedElementList.insert( expectedInsertIt, 3, *newElList.begin() );

            verifyReadOnlySequence( coll, newExpectedElementList );
        }
    }

    SECTION("new")
    {
        coll.insertNewAtBegin( std::forward<ConstructArgs>(constructArgs)... );
        newExpectedElementList.insert( expectedInsertIt, expectedConstructedEl );

        verifyReadOnlySequence( coll, newExpectedElementList );
    }
}


    
template<class CollType, typename... ConstructArgs>
static void verifySequence(
    CollType& coll,
    std::list<typename CollType::Element> expectedElementList,
    std::initializer_list<typename CollType::Element> newElList,
    std::function< bool(const typename CollType::Element&) > isMovedRemnant,
    typename CollType::Element expectedConstructedEl,
    ConstructArgs... constructArgs
    )
{
    verifyReadOnlySequence(coll, expectedElementList);

    std::list<typename CollType::Element> newExpectedElementList( expectedElementList );

    SECTION("clear")
    {
        coll.clear();

        verifyReadOnlySequence( coll, {});
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
                    verifyReadOnlySequence( coll, {});
                }

                SECTION("move")
                {
                    coll = std::move(otherColl);
                    verifyReadOnlySequence( coll, {});

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
                    verifyReadOnlySequence( coll, {expectedConstructedEl} );
                }

                SECTION("move")
                {
                    coll = std::move(otherColl);
                    verifyReadOnlySequence( coll, {expectedConstructedEl} );

                    REQUIRE( otherColl.size() == 0);
                }
            }
        }

        SECTION("initializer_list")
        {
            SECTION("empty")
            {
                coll = {};
                verifyReadOnlySequence( coll, {});
            }

            SECTION("non-empty")
            {
                coll = {expectedConstructedEl};

                verifyReadOnlySequence( coll, {expectedConstructedEl} );
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
                    
                verifyReadOnlySequence( coll, newExpectedElementList);
            }

            SECTION("last")
            {
                coll.removeAt( --coll.end() );
                newExpectedElementList.erase( --newExpectedElementList.end() );                   
                    
                verifyReadOnlySequence( coll, newExpectedElementList);
            }

            if( expectedElementList.size() > 1)
            {
                SECTION("mid")
                {
                    coll.removeAt( ++coll.begin() );
                    newExpectedElementList.erase( ++newExpectedElementList.begin() );                   
                    
                    verifyReadOnlySequence( coll, newExpectedElementList);
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
                    
            verifyReadOnlySequence( coll, newExpectedElementList);
        }
    }

    SECTION("insertAt")
    {
        SECTION("at begin")
            verifyInsertAt(coll, 0, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );

        if(expectedElementList.size()>1)
        {
            SECTION("in middle")
                verifyInsertAt(coll, 1, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
        }

        SECTION("at end")
            verifyInsertAt(coll, expectedElementList.size(), expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
    }

    SECTION("add")
    {
        verifyAdd(coll, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
    }

    SECTION("insertAtBegin")
    {
        verifyInsertAtBegin(coll, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)... );
    }

    SECTION("setSize")
    {
        SECTION("to 0")
        {
            coll.setSize(0);
            newExpectedElementList.clear();

            verifyReadOnlySequence( coll, newExpectedElementList);
        }

        if(expectedElementList.size()>0)
        {
            SECTION("to one less")
            {
                coll.setSize( coll.size()-1 );
                newExpectedElementList.erase( --newExpectedElementList.end() );

                verifyReadOnlySequence( coll, newExpectedElementList);
            }
        }

        SECTION("to one more")
        {
            coll.setSize( coll.size()+1 );
            newExpectedElementList.push_back( typename CollType::Element() );

            verifyReadOnlySequence( coll, newExpectedElementList);
        }

        SECTION("to 100 more")
        {
            coll.setSize( coll.size()+100 );

            for(int i=0; i<100; i++)
                newExpectedElementList.push_back( typename CollType::Element() );

            verifyReadOnlySequence( coll, newExpectedElementList);
        }
    }
}

template<class CollType, class ItType >
void verifyIndexedAccess(CollType& coll, int index, ItType expectedElementIt )
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
void testIndexedAccess(CollType& coll)
{
    SECTION("0")
        verifyIndexedAccess( coll, 0, coll.begin() );
            
    SECTION("1")
        verifyIndexedAccess( coll, 1, ++coll.begin() );

    SECTION("last")
        verifyIndexedAccess( coll, coll.size()-1, --coll.end() );

    SECTION("end")
        verifyIndexedAccess( coll, coll.size(), coll.end() );
}

template<class CollType>
void testPrepareForSize(CollType& coll)
{
    std::list< typename CollType::Element > origElements( coll.begin(), coll.end() );

    typename CollType::Size origCapacity = coll.capacity();

    typename CollType::Size prepareFor=0;
    
    SECTION("0")
        prepareFor = 0;

    SECTION("1")
        prepareFor = 1;

    SECTION("size")
        prepareFor = coll.size();

    SECTION("capacity")
        prepareFor = coll.capacity();
    
    SECTION("capacity+1")
        prepareFor = coll.capacity()+1;

    coll.prepareForSize( prepareFor );

    // prepareForSize should NEVER change the collection contents
    verifyReadOnlySequence( coll, origElements );

    typename CollType::Size newCapacity = coll.capacity();

    REQUIRE( newCapacity>=coll.size() );

    if(prepareFor <= origCapacity)
        REQUIRE( newCapacity <= origCapacity );

    REQUIRE( newCapacity >= prepareFor );
}

template<typename ElType, typename... ConstructArgs>
static void testArray(
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
                Array<ElType> coll( 0, *newElList.begin() );
                
                verifyReadOnlySequence( coll, expectedElements );
            }

            SECTION("3")
            {
                Array<ElType> coll( 3, *newElList.begin() );

                for(int i=0; i<3; i++)
                    expectedElements.push_back( *newElList.begin() );

                verifyReadOnlySequence( coll, expectedElements );
            }
        }

        SECTION("n default constructed")
        {
            SECTION("0")
            {
                Array<ElType> coll( 0 );
                verifyReadOnlySequence( coll, expectedElements );
            }

            SECTION("3")
            {
                Array<ElType> coll( 3 );

                for(int i=0; i<3; i++)
                    expectedElements.push_back( ElType() );

                verifyReadOnlySequence( coll, expectedElements );
            }
        }

        SECTION("iterators")
        {
            SECTION("empty")
            {
                Array<ElType> coll( newElList.begin(), newElList.begin() );
                verifyReadOnlySequence( coll, expectedElements );
            }

            SECTION("non-empty")
            {
                Array<ElType> coll( newElList.begin(), newElList.end() );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, expectedElements );
            }
        }

        SECTION("copy")
        {
            SECTION("Array")
            {
                Array<ElType> src( newElList );

                Array<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, expectedElements );
            }

            SECTION("vector")
            {
                std::vector<ElType> src( newElList );

                Array<ElType> coll(src);

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );

                verifyReadOnlySequence( coll, expectedElements );
            }
        }

        SECTION("move")
        {
            SECTION("Array")
            {
                Array<ElType> src( newElList );

                Array<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                verifyReadOnlySequence( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }

            SECTION("vector")
            {
                std::vector<ElType> src( newElList );

                Array<ElType> coll( std::move(src) );

                expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
                verifyReadOnlySequence( coll, expectedElements );

                REQUIRE( src.size()==0 );
            }
        }

        SECTION("initializer_list")
        {
            Array<ElType> coll( newElList );

            expectedElements.insert( expectedElements.begin(), newElList.begin(), newElList.end() );
            verifyReadOnlySequence( coll, expectedElements );
        }
    }

    Array<ElType> coll;

    SECTION("empty")
    {
        verifySequence(
            coll,
            std::list<ElType>({}),
            newElList,
            isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)... );

        SECTION("prepareForSize")
            testPrepareForSize(coll);
    }

    SECTION("non-empty")
    {
        for(auto& el: initElList)
            coll.add( el );

        verifySequence(
            coll,
            std::list<ElType>(initElList),
            newElList,
            isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)... );        

        SECTION("indexed access")
        {
            SECTION("normal")
                testIndexedAccess(coll);

            SECTION("const")
                testIndexedAccess( (const Array<ElType>&) coll );
        }

        SECTION("getData")
        {
            SECTION("normal")
                REQUIRE( coll.getData() == &coll[0] );

            SECTION("const")
                REQUIRE( ((const Array<ElType>&)coll).getData() == &coll[0] );
        }

        SECTION("prepareForSize")
            testPrepareForSize(coll);
    }    
}

template<class CollType, class ItType>
static void verifyFindForSpecificVariant(CollType& coll, typename CollType::Element toFind, ItType expectedResult )
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
static void verifyFind(CollType& coll, typename CollType::Element toFind, typename CollType::Iterator expectedResult )
{
    SECTION("normal")
        verifyFindForSpecificVariant<CollType, typename CollType::Iterator>( coll, toFind, expectedResult);
    
    SECTION("const")
    {
        const CollType& constColl(coll);

        // convert the const iterator to non-const
        typename CollType::ConstIterator constExpectedResult = constColl.begin();
        std::advance( constExpectedResult, std::distance(coll.begin(), expectedResult ) );

        verifyFindForSpecificVariant<const CollType, typename CollType::ConstIterator>( constColl, toFind, constExpectedResult);
    }
}




template<class CollType, class ItType>
static void verifyReverseFindForSpecificVariant(CollType& coll, typename CollType::Element toFind, ItType expectedResult, ItType posAfterExpectedPos )
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
static void verifyReverseFind(CollType& coll, typename CollType::Element toFind, typename CollType::Iterator expectedResult )
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
static void verifyFindCondition(CollType& coll, std::function< bool(const typename CollType::Element&) > conditionFunc, typename CollType::Iterator expectedResult )
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
static void verifyReverseFindCondition(CollType& coll, std::function< bool(const typename CollType::Element&) > conditionFunc, typename CollType::Iterator expectedResult )
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
static void testFind(std::initializer_list<typename CollType::Element> elements, const typename CollType::Element& elNotInColl )
{
    SECTION("find")
    {
        CollType coll;

        SECTION("empty")
        {
            SECTION("not found")
                verifyFind( coll, elNotInColl, coll.end() );
        }

        SECTION("not empty")
        {        
            coll.addSequence(elements);

            // the first element must equal the third one for these tests to work
            REQUIRE( coll.getFirst() == *(++(++coll.begin())) );

            SECTION("first")
                verifyFind( coll, coll.getFirst(), coll.begin() );

            SECTION("last")
                verifyFind( coll, coll.getLast(), --coll.end() );

            SECTION("second")
                verifyFind( coll, *++coll.begin(), ++coll.begin() );

            SECTION("not found")
                verifyFind( coll, elNotInColl, coll.end() );
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
void verifySortResult( CollType& coll, std::initializer_list<typename CollType::Element> elements, bool expectInvertedOrder = false )
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
void testSort(
    std::initializer_list<typename CollType::Element> elements,
    std::initializer_list<typename CollType::Element> stableSortedElements )
{
    SECTION("empty")
    {
        CollType coll;

        SECTION("sort")
        {
            coll.sort();
            verifyReadOnlySequence( coll, {} );
        }

        SECTION("sort with compare func")
        {
            coll.sort(
                [](typename CollType::Element& a, typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            verifyReadOnlySequence( coll, {} );
        }

        SECTION("stableSort")
        {
            coll.stableSort();
            verifyReadOnlySequence( coll, {} );
        }

        SECTION("stableSort with compare func")
        {
            coll.stableSort(
                [](const typename CollType::Element& a, const typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            verifyReadOnlySequence( coll, {} );
        }
    }

    SECTION("not empty")
    {
        CollType coll( elements );

        SECTION("sort")
        {
            coll.sort();

            verifySortResult( coll, elements );
        }

        SECTION("sort with compare func")
        {
            coll.sort(
                [](typename CollType::Element& a, typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            verifySortResult( coll, elements, true );
        }

        SECTION("stableSort")
        {
            coll.stableSort();

            verifySortResult( coll, elements );

            verifyReadOnlySequence( coll, stableSortedElements );
        }

        SECTION("stableSort with compare func")
        {
            coll.stableSort(
                [](const typename CollType::Element& a, const typename CollType::Element& b)
                {
                    // use inverted comparison
                    return (b<a);
                } );

            verifySortResult( coll, elements, true );

            std::list<typename CollType::Element> expectedSortedElements( stableSortedElements.begin(), stableSortedElements.end() );
            expectedSortedElements.reverse();

            verifyReadOnlySequence( coll, expectedSortedElements );
        }
    }
}


TEST_CASE("Array")
{
    SECTION("simple type")
    {
        testArray<int>(
            {17, 42, 3},
            {100, 101, 102},
            [](const int& el)
            {
                return true;
            },
            345,
            345 );       

        testFind< Array<int> >( {17, 42, 17, 3}, 88 );
        testSort< Array<int> >( {17, 42, 17, 3}, {3, 17, 17, 42} );
    }

    SECTION("complex type")
    {
        SECTION("ordered")
        {
            testArray<TestOrderedArrayElement_>(
                { TestOrderedArrayElement_(17, 117),
                  TestOrderedArrayElement_(42, 142),
                  TestOrderedArrayElement_(3, 103)
                },
                { TestOrderedArrayElement_(100, 201),
                  TestOrderedArrayElement_(102, 202),
                  TestOrderedArrayElement_(103, 203)
                },
                [](const TestOrderedArrayElement_& el)
                {
                    return el._a==-2 && el._b==-2;
                },
                TestOrderedArrayElement_(345, 456),
                345, 456 );

            testFind< Array<TestOrderedArrayElement_> >(
                { TestOrderedArrayElement_(17, 117),
                    TestOrderedArrayElement_(42, 142),
                    TestOrderedArrayElement_(17, 117),
                    TestOrderedArrayElement_(3, 103),
                },
                TestOrderedArrayElement_(400, 401) );

            testSort< Array<TestOrderedArrayElement_> >(
                { TestOrderedArrayElement_(17, 1),
                    TestOrderedArrayElement_(42, 142),
                    TestOrderedArrayElement_(17, 2),
                    TestOrderedArrayElement_(3, 103),
                },
                { TestOrderedArrayElement_(3, 103),
                    TestOrderedArrayElement_(17, 1),
                    TestOrderedArrayElement_(17, 2),
                    TestOrderedArrayElement_(42, 142),
                } );
        }

        SECTION("unordered comparable")
        {
            testArray<TestUnorderedComparableArrayElement_>(
                { TestUnorderedComparableArrayElement_(17, 117),
                  TestUnorderedComparableArrayElement_(42, 142),
                  TestUnorderedComparableArrayElement_(3, 103)
                },
                { TestUnorderedComparableArrayElement_(100, 201),
                  TestUnorderedComparableArrayElement_(102, 202),
                  TestUnorderedComparableArrayElement_(103, 203)
                },
                [](const TestUnorderedComparableArrayElement_& el)
                {
                    return el._a==-2 && el._b==-2;
                },
                TestUnorderedComparableArrayElement_(345, 456),
                345, 456 );

            testFind< Array<TestUnorderedComparableArrayElement_> >(
                { TestUnorderedComparableArrayElement_(17, 117),
                    TestUnorderedComparableArrayElement_(42, 142),                
                    TestUnorderedComparableArrayElement_(17, 117),
                    TestUnorderedComparableArrayElement_(3, 103),
                },
                TestUnorderedComparableArrayElement_(400, 401) );

            // cannot sort because not ordered
        }

        SECTION("unordered uncomparable")
        {
            testArray<TestArrayElement_>(
                { TestArrayElement_(17, 117),
                  TestArrayElement_(42, 142),
                  TestArrayElement_(3, 103)
                },
                { TestArrayElement_(100, 201),
                  TestArrayElement_(102, 202),
                  TestArrayElement_(103, 203)
                },
                [](const TestArrayElement_& el)
                {
                    return el._a==-2 && el._b==-2;
                },
                TestArrayElement_(345, 456),
                345, 456 );

            // cannot use Array::find, since elements are not comparable
        }
    }
}



