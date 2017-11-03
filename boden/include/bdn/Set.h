#ifndef BDN_Set_H_
#define BDN_Set_H_

#include <set>

#include <bdn/StdCollection.h>
#include <bdn/SequenceFilter.h>

namespace bdn
{

/** A container that holds a set of unique elements (without duplicates). New elements are only
    added when they are not yet in the set.

	Element ordering
	----------------

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

	Iteration order
	---------------

	Iterators of the Set class (see begin()) return the elements in "small first" order (according
	to the element ordering that the Map uses - see above).


	Base classes and std::set compatibility
	---------------------------------------
        
    The Set class is derived from std::set and is 100 % compatible with it.
    It can be used as a drop-in replacement.

    Set provides renamed aliases for some std::set methods and types to ensure that
    they match the naming conventions used otherwise in the framework. The new
    names are also intended to make using the class more intuitive for newcomers that may not be familiar with
    the C++ standard library.

    Note that Set is also derived from bdn::Base, so it can be used with smart pointers (see bdn::P).

	Allocator
	---------
    
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
    
	
    /** The class of iterator objects for iteration over the collection elements in reverse order (with read/write access to the elements).*/
    typedef typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::reverse_iterator ReverseIterator;

    /** The class of iterator objects for iteration over the collection elements in reverse order (with read-only access to the elements).*/
    typedef typename StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::const_reverse_iterator ConstReverseIterator;



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
        : Set( static_cast<const std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR>&>( other ) )
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
        std::set object.
        
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


		
    /** Constructs a new element and adds it to the set, if it not yet in the set.
        The arguments passed to addNew are passed on to the constructor of the
        newly constructed element.
        
        This function can be used instead of add() when one wants to add a newly
        created value more efficiently. With add() the new value would initially be constructed
        as a temporary variable and then a copy or move operation would be made 
        to bring it into the collection. addNew constructs the new element directly inside
        the collection, avoiding any copying or moving.

        If a new element was added then a reference to that element is returned.
		If the element was already in the set then a reference to the pre-existing element is returned.
        */
    template< class... Args > 
    const Element& addNew( Args&&... args )
    {
        std::pair<iterator,bool> result = StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::emplace( std::forward<Args>(args)... );

		return *result.first;
    }




	/** In general, this collection operation prepares the collection for a bigger insert operation.
        This function is provided for consistency with other collection types only - for bdn::Set this
        function has no effect and does nothing.
    */
    void prepareForSize(Size size)
    {
        // do nothing
    }


    /** Returns true if the set contains the specified element.

        The function uses the set's compare function to determine if a set element is equal to 
        the \c el parameter. The two elements are considered to be equal if neither
        one is "smaller" than the other.
    */
    bool contains( const Element& el ) const
    {
        return ( StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::count(el) != 0);
    }


	
    class ElementMatcher_
    {
    public:
        ElementMatcher_(const Element& elementToFind)
            : _element( elementToFind )
        {
        }

        void operator() (Set& set, Iterator& it)
        {
            // note that the "it" parameter is NEVER equal to end() when we are called.
            // That also means that we are never called for empty maps.

            if( it==set.begin() )
                it = set.find( _element );
            else
                it = set.end();
        }

    private:
        Element _element;
    };

	template<typename MatchFuncType>
    class FuncMatcher_
    {
    public:
        FuncMatcher_( MatchFuncType matchFunc )
            : _matchFunc( matchFunc )
        {
        }

        void operator() (Set& set, Iterator& it)
        {
            // note that the "it" parameter is NEVER equal to end() when we are called.
            // That also means that we are never called for empty maps.

            while( ! _matchFunc(*it) )
            {
                ++it;
                if( it==set.end() )
                    break;
            }
        }    

	private:
		MatchFuncType _matchFunc;
    };

	using ElementFinder = SequenceFilter<Set, ElementMatcher_>;

	template<typename MatchFuncType>
	using FuncFinder = SequenceFilter<Set, FuncMatcher_<MatchFuncType> >;

	
	ElementFinder findAll(const Element& elToFind)
	{
        return ElementFinder(*this, ElementMatcher_(elToFind) );
	}

	
	template<class MatchFuncType>
	FuncFinder<MatchFuncType> findAllCustom( MatchFuncType matchFunction )
	{
		return FuncFinder<MatchFuncType>(*this, FuncMatcher_<MatchFuncType>(matchFunction) );
	}





    /** Searches for the specified element in the set.
        The search uses the set's compare function to determine if a set element is equal to 
        the \c toFind parameter. The two elements are considered to be equal if neither
        one is "smaller" than the other.

        find is very fast in the Set class. Its complexity is logarithmic to the size of the set.
            
        Returns an iterator to the found element, or end() if no such element is found.
    */
	Iterator find( const Element& toFind )
	{
        return StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::find( toFind );
	}


    /** Const version of find() - returns a read-only iterator.
    */
	ConstIterator find( const Element& toFind ) const
	{
        return StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::find( toFind );
	}

	
    /** Searches for the first element for which the specified condition function returns true.        
        conditionFunc must take a collection element reference as its only parameter and return a boolean.

        Note that for the Set class findCondition() is a lot slower than find(). find can take advantage
        of the internal data structures of the set to find the element in logarithmic time. findCondition 
        on the other hand needs to check potentially all elements in the set (i.e. it has linear complexity).
            
        Returns an iterator to the found element, or end() if no such element is found.
    */
    template<class ConditionFuncType>
	Iterator findCondition(ConditionFuncType conditionFunc )
	{
        return std::find_if( this->begin(), this->end(), conditionFunc );
	}


    /** Const version of findCondition() - returns a read-only iterator.
    */
    template<class ConditionFuncType>
	ConstIterator findCondition(ConditionFuncType conditionFunc ) const
	{
        return std::find_if( this->begin(), this->end(), conditionFunc );
	}



    /** If the set contains the specified element, remove it. Does nothing if the element
        is not in the set.*/
    void findAndRemove(const Element& val)
    {
        StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::erase( val );
    }


    
    /** Removes all elements for which the specified function checkFunc returns true.
    
        checkFunc must be a function that takes a reference to a set element as its parameter
        and returns true if the element should be removed.
    */
    template<typename CheckFuncType>
    void findConditionAndRemove( CheckFuncType& checkFunc )
    {
        for(auto it = begin(); it!=end(); )
        {
            if( checkFunc(*it) )
                it = StdCollection< std::set<ELTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::erase( it );
            else
                ++it;
        }
    }

        
	

    /** Returns an iterator that is intended for iteration over the collection elements
        in reversed order.

        The returned iterator points to the last element of the collection.

        The directions of its ++ and -- operator are inverted: the ++ operator moves
        the iterator to the previous element, -- moves it to the next element.

        Reverse iterators should only be compared to other reverse iterators.
        reverseEnd() returns an iterator that marks the end point of the reverse iteration
        (analogous to end() for forward iteration).

        The following code example iterates over the collection elements in reversed order:

        \code

        Array<int> ar = {1, 5, 7};

        auto it = ar.reverseBegin();    // iterator initially points to the last element
        while( it != ar.reverseEnd() )
        {
            // do something with the element
            ...

            // go to the previous element of the collection
            ++it;
        }       

        \endcode

        */
    ReverseIterator reverseBegin() noexcept
	{
		return this->rbegin();
	}


    /** Const version of reverseBegin() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator reverseBegin() const noexcept
	{
		return this->rbegin();
	}


    /** Const version of reverseBegin() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator constReverseBegin() const noexcept
	{
		return this->crbegin();
	}


    /** Marks the end point of a reverse iteration.

        This is used together with reverseBegin() to iterate over the collection elements in reverse order.
        It behaves analogous to end(): it points to a collection position just before the first element.
        
        Like end(), the iterator returned by reverseEnd() does not point to a valid element - it simply marks the
        end position for the reverse iteration. See the documentation of reverseBegin() for more information
        on how to use it.
    */
    ReverseIterator reverseEnd() noexcept
	{
		return this->rend();
	}


    /** Const version of reverseEnd() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator reverseEnd() const noexcept
	{
		return this->rend();
	}


    /** Const version of reverseEnd() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator constReverseEnd() const noexcept
	{
		return this->crend();
	}


};


}


#endif
