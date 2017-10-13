#ifndef BDN_Set_H_
#define BDN_Set_H_

#include <set>

#include <bdn/StdCollection.h>

namespace bdn
{

/** A container that holds a set of unique elements (without duplicates). New elements are only
    added when they are not yet in the set.

    The Set class requires that there is a well defined ordering among the elements. Any ordering is fine,
    but Set must be able to determine if element A comes before element B in that ordering.
    This information is used to optimize the element storage, and to determine if an element is a duplicate of one
    that is already in the set. Two elements A and B are considered to be duplicates of one another if the ordering
    considers them to be "equal" (i.e. neither does A come before B, nor B before A - so they effectively occupy the same
    "slot" in the ordering).

    By default the Set class uses the < operator on the elements to determine their ordering.
    This can be changed with the second template parameter (for example, if the elements are objects that
    do not support the < operator). If specified, the second template parameter must be the
    type of a compare function object. An instance of this compare function object type can be passed to the
    constructor of Set. If no compare object is passed to the constructor then one is automatically
    created.

    Elements A and B are considered to be duplicates of each other, if neither A < B not B < A, with "<" being
    the ordering of the elements in the set.
        
    The Set class is derived from std::set and is 100 % compatible with it.
    It can be used as a drop-in replacement.

    Set provides renamed aliases for some std::set methods and types to ensure that
    they match the naming conventions used otherwise in the framework. The new
    names are also intended to make using the class more intuitive for newcomers that may not be familiar with
    the C++ standard library.

    Note that Set is also derived from bdn::Base, so it can be used with smart pointers (see bdn::P).
    
    Like the C++ standard library collections, one can specify an "allocator" type as an optional
    third template argument. This is used when special custom memory management is needed.
    If you do not need that then you should ignore the ALLOCATOR template argument and leave it at the default. 

*/
template<typename ELTYPE, typename COMPAREFUNCTYPE = std::less<ELTYPE>, class ALLOCATOR = std::allocator<ELTYPE> >
class Set : public StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >
{
public:
    
    using typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::Element;
    using typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::Size;
    using typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::Iterator;
    using typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::ConstIterator;
    using typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::ReverseIterator;
    using typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::ConstReverseIterator;

    Set()
        : Set( COMPAREFUNCTYPE() )
    {
    }

    explicit Set( const COMPAREFUNCTYPE& compareFunc, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( compareFunc, alloc )
    {
    }

    explicit Set( const ALLOCATOR& alloc )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( alloc )
    {
    }


    template< class InputIt >
    Set(    InputIt beginIt,
            InputIt endIt,
            const COMPAREFUNCTYPE& compareFunc = COMPAREFUNCTYPE(),
            const ALLOCATOR& alloc = ALLOCATOR() )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( beginIt, endIt, compareFunc, alloc )
    {
    }

    template< class InputIt >
    Set(    InputIt beginIt,
            InputIt endIt,
            const ALLOCATOR& alloc)
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( beginIt, endIt, COMPAREFUNCTYPE(), alloc)
    {
    }


    Set( const Set& other )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( other )
    {
    }

    Set( const std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>& other )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( other )
    {
    }

    Set( const Set& other, const ALLOCATOR& alloc )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( other, alloc )
    {
    }

    Set( const std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>& other, const ALLOCATOR& alloc )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( other, alloc )
    {
    }


    Set( Set&& other )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move( static_cast<std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>&&>(other) ) )
    {
    }

    Set( std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>&& other )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move(other) )
    {
    }

    Set( Set&& other, const ALLOCATOR& alloc )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move( static_cast<std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>&&>(other) ), alloc )
    {
    }

    Set( std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>&& other, const ALLOCATOR& alloc )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move(other), alloc )
    {
    }



    Set(    std::initializer_list<ELTYPE> initList,
            const COMPAREFUNCTYPE& compFunc = COMPAREFUNCTYPE(),
            const ALLOCATOR& alloc = ALLOCATOR() )
        : StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( initList, compFunc, alloc )
    {
    }

    Set(    std::initializer_list<ELTYPE> initList,
            const ALLOCATOR& alloc )
        : Set(initList, COMPAREFUNCTYPE(), alloc)
    {
    }


    /** Replaces the current contents of the set with copies of the elements from the specified other
        Set.
        
        Returns a reference to this Set object.
        */
    Set& operator=( const Set& other )
    {
        std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


    /** Replaces the current contents of the set with copies of the elements from the specified
        vector.
        
        Returns a reference to this Set object.
        */
    Set& operator=( const std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>& other )
    {
        std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


     /** Replaces the current contents of the set with copies of the elements from the specified
        initializer list. This is called by the compiler if a  "= {...} " statement is used. For example:

        \code
        Set<int> mySet;

        mySet = {1, 17, 42};
        \endcode

        Returns a reference to this Set object.
        */
    Set& operator=( std::initializer_list<Element> initList )
    {
        std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=(initList);
        return *this;
    }  




    /** Moves the data from the specified other Set object to this set, replacing any current contents in the process.
        The other Set object is invalidated by this operation.
        */
    Set& operator=( Set&& other )
        noexcept( ALLOCATOR::propagate_on_container_move_assignment::value || ALLOCATOR::is_always_equal::value )
    {
        std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }
    

    /** Moves the data from the specified other Set object to this set, replacing any current contents in the process.
        The other Set object is invalidated by this operation.
        */
    Set& operator=( std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>&& other )
        noexcept( ALLOCATOR::propagate_on_container_move_assignment::value || ALLOCATOR::is_always_equal::value )
    {
        std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }

    
    
    /** Adds the specified element if it is not yet in the set.
        Does nothing if the element is already in the set.

        Returns true if the element was added, false if it was already in the set.
        */
    bool add( const Element& value )
    {
        std::pair<iterator,bool> result = StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert(value);

        return result.second;
    }


    /** Like add(), but instead of the new element being a copy of the specified
        element, the C++ move semantics are used to move the element data from the parameter
        \c el to the new collection element.

        Returns true if the element was added, false if it was already in the set.
        */
    bool add( Element&& value )
    {
        std::pair<iterator,bool> result = StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert( std::move(value) );

        return result.second;
    }



    

    /** Adds the elements from the specified [beginIt ... endIt)
        iterator range to the set.
        endIt points to the location just *after* the last element to add.

        The beginIt and endIt iterators must not refer to the target set. They can be from an arbitrary other
        collection -- also from a collection of a different type. The only condition is that the element type of the
        source collection must be compatible to the element type of the target set.
        */
    template< class InputIt >
    void addSequence( InputIt beginIt, InputIt endIt )
    {
        StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert( beginIt, endIt);
    }


    /** Adds the elements from the specified initializer list to the collection.

        This can be used to add multiple elements with the {...} notation. For example:

        \begin
        Set<int> mySet;

        mySet.addSequence( {1, 4, 7} );    // adds three elements to the set

        */
    void addSequence( std::initializer_list<Element> initList )
    {
        StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert( initList );
    }




    /** Returns a reference to the first (=smallest) element of the set. Throws OutOfRangeError if the collection
        is empty.*/
    const Element& getFirst() const
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdSequenceCollection::getFirst called on empty collection.");

        return *StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::begin();
    }



    /** Returns a reference to the last (biggest) element of the set. Throws OutOfRangeError if the collection
        is empty.*/
    const Element& getLast() const
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdSequenceCollection::getLast called on empty collection.");

        return *--StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::end();
    }



    void removeFirst()
    {
    }
        

};


}


#endif
