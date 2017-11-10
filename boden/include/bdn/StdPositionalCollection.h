#ifndef BDN_StdPositionalCollection_H_
#define BDN_StdPositionalCollection_H_

#include <bdn/StdCollection.h>
#include <bdn/SequenceFilter.h>
#include <bdn/sort.h>


namespace bdn
{


/** Base class for collections based on standard C++ collections 
	which give the user complete control over the position of each eleemnt 
	(std::vector, std::list, ...)
*/
template<class BaseCollectionType >
class StdPositionalCollection : public StdCollection< BaseCollectionType >
{
public:
    
    using typename StdCollection< BaseCollectionType >::Element;
    using typename StdCollection< BaseCollectionType >::Size;
    using typename StdCollection< BaseCollectionType >::Iterator;
    using typename StdCollection< BaseCollectionType >::ConstIterator;
    
    /** The class of iterator objects for iteration over the collection elements in reverse order (with read/write access to the elements).*/
    typedef typename BaseCollectionType::reverse_iterator ReverseIterator;

    /** The class of iterator objects for iteration over the collection elements in reverse order (with read-only access to the elements).*/
    typedef typename BaseCollectionType::const_reverse_iterator ConstReverseIterator;

    
    template<class... Args>
    StdPositionalCollection(Args... args)
     : StdCollection<BaseCollectionType>( std::forward<Args>(args)... )
    {
    }

    
    /** Returns a reference to the first element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    Element& getFirst()
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdPositionalCollection::getFirst called on empty collection.");

        return BaseCollectionType::front();
    }

    /** Returns a const reference to the first element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    const Element& getFirst() const
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdPositionalCollection::getFirst called on empty collection.");

        return BaseCollectionType::front();
    }



    /** Returns a reference to the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    Element& getLast()
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdPositionalCollection::getLast called on empty collection.");

        return BaseCollectionType::back();
    }


    /** Returns a const reference to the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    const Element& getLast() const
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdPositionalCollection::getLast called on empty collection.");

        return BaseCollectionType::back();
    }



    
    
    /** Adds a new element to the collection (at the end).
    */
    void add( const Element& value )
    {
        BaseCollectionType::push_back(value);
    }


    /** Like add(), but instead of the new element being a copy of the specified
        element, the C++ move semantics are used to move the element data from the parameter
        \c el to the new collection element.
        */
    void add( Element&& value )
    {
        BaseCollectionType::push_back( std::forward<Element>(value) );
    }


    /** Adds \c count copies copy of the specified element \c el to the collection (at the end).        
    */
    void addMultipleCopies( Size count, const Element& el)
    {
        BaseCollectionType::insert(BaseCollectionType::end(), count, el);
    }


    /** Adds the elements from the specified [beginIt ... endIt)
        iterator range to the collection (at the end).
        endIt points to the location just *after* the last element to add.

        The beginIt and endIt iterators must not refer to the target collection. They can be from an arbitrary other
        collection -- also from a collection of a different type. The only condition is that the element type of the
        source collection must be compatible to the element type of the target collection.
        */
    template< class InputIt >
    void addSequence( InputIt beginIt, InputIt endIt )
    {
        BaseCollectionType::insert( this->end(), beginIt, endIt);
    }


    /** Adds the elements from the specified initializer list to the collection (at the end).

        This can be used to add multiple elements with the {...} notation. For example:

        \begin
        Array<int> ar;

        ar.addSequence( {1, 4, 7} );    // adds three elements to the array

        */
    void addSequence( std::initializer_list<Element> initList )
    {
        BaseCollectionType::insert( this->end(), initList );
    }


	/** Adds the elements from the specified source \ref sequence.md "sequence" to the collection.
		
		Since all collections are also sequences, this can be used to copy all elements from
		any other collection of any type, as long as it has a compatible element type.
		
		\code

		Array< long > myArray;

		// note that the element types do not even have to be equal, only compatible.
		// Here the source list uses int, while the destination array uses long.
		List< int > sourceList( { 1, 9, 3} );
		
		// add all elements from sourceList to myArray
		myArray.addSequence( sourceList );
		
		\endcode
        */
	template<class SequenceType>
    void addSequence( const SequenceType& sequence )
    {
        BaseCollectionType::insert( this->end(), sequence.begin(), sequence.end() );
    }



    /** Constructs a new element and adds it to the collection (at the end).
        The arguments passed to addNew are passed on to the constructor of the
        newly constructed element.
        
        This function can be used instead of add() when one wants to add a newly
        created value more efficiently. With add() the new value would initially be constructed
        as a temporary variable and then a copy or move operation would be made 
        to bring it into the collection. addNew constructs the new element directly inside
        the collection, avoiding any copying or moving.

        Returns a reference to the newly added element.
        */
    template< class... Args > 
    Element& addNew( Args&&... args )
    {
        BaseCollectionType::emplace_back( std::forward<Args>(args)... );
        return BaseCollectionType::back();
    }



    
    /** Inserts a copy of the specified element \c el at position \c pos.
        
        If \c pos points to an element of the collection then the new element is inserted
        before that element.

        If \c pos points to the collection end then the new element is added at the end.

        Returns an iterator to the newly inserted element.                
    */
    Iterator insertAt( ConstIterator pos, const Element& el)
    {
        return BaseCollectionType::insert(pos, el);
    }


    /** Like insertAt() but instead of being a copy the newly inserted element is created
        with C++ move semantics from the \c el argument.        
    */
    Iterator insertAt( ConstIterator pos, Element&& el)
    {
        return BaseCollectionType::insert(pos, std::forward<Element>(el) );
    }


    /** Inserts \c count copies copy of the specified element \c el at the specified position \pos.
        
        If \c pos points to an element of the collection then the new elements are inserted
        before that element.

        If \c pos points to the collection end then the new elements are added at the end.

        Returns an iterator to first inserted element. If count is 0 then \c pos is returned.
    */
    Iterator insertMultipleCopiesAt( ConstIterator pos, Size count, const Element& el)
    {
        return BaseCollectionType::insert(pos, count, el);
    }


    /** Inserts copies of the elements from the specified [beginIt ... endIt)
        iterator range at position \c pos.
        endIt points to the location just *after* the last element to add.

        The beginIt and endIt iterators must NOT refer to the target collection that is being changed. They can be from an arbitrary other
        collection -- also from a collection of a different type. The only condition is that the element type of the
        source collection must be compatible to the element type of the target collection.

        If \c pos points to an element of the collection then the new elements are inserted
        before that element.

        If \c pos points to the collection end then the new elements are added at the end.

        Returns an iterator to first inserted element. If no elements are inserted (beginIt == endIt)
        then \c pos is returned.
        */
    template< class InputIt >
    Iterator insertSequenceAt( ConstIterator pos, InputIt beginIt, InputIt endIt )
    {
        return BaseCollectionType:: template insert<InputIt>(pos, beginIt, endIt);
    }



    /** Inserts the elements from the specified initializer list at position \c pos.

        If \c pos points to an element of the collection then the new elements are inserted
        before that element.

        If \c pos points to the collection end then the new elements are added at the end.

        This can be used to insert multiple elements with the {...} notation. For example:

        \begin
        Array<int> ar;

        ar.insertSequenceAt( ar.begin(), {1, 4, 7} );    // inserts three elements at the start of the array

        */
    Iterator insertSequenceAt( ConstIterator pos, std::initializer_list<Element> initList )
    {
        return BaseCollectionType::insert(pos, initList);
    }



    /** Constructs a new element and inserts it to the collection at position \c pos.
        Any arguments passed to insertNew after \c pos are passed on to the constructor of the
        newly constructed element.
        
        This function can be used instead of inset() when one wants to insert a newly
        created value more efficiently. With insert() the new value would initially be constructed
        as a temporary variable and then a copy or move operation would be made 
        to bring it into the collection. insertNew constructs the new element directly inside
        the collection, avoiding any copying or moving.

        Returns an iterator to the newly inserted element.
        */
    template< class... Args > 
    Iterator insertNewAt( ConstIterator pos, Args&&... args )
    {
        return BaseCollectionType::emplace(pos, std::forward<Args>(args)... );
    }

    

    /** Inserts an element at the beginning of the collection.

        Same as insert( begin(), el).    
    */
    void insertAtBegin( const Element& el )
    {
        BaseCollectionType::insert( this->begin(), el);
    }

    /** Inserts an element at the beginning of the collection.

        Same as insert( begin(), el).    
    */
    void insertAtBegin( Element&& el )
    {
        BaseCollectionType::insert( this->begin(), std::forward<Element>(el) );
    }


    
    /** Inserts \c count copies copy of the specified element \c el at the beginning of the collection.
        
        Returns an iterator to first inserted element. If count is 0 then \c begin() is returned.
    */
    Iterator insertMultipleCopiesAtBegin( Size count, const Element& el)
    {
        return BaseCollectionType::insert( this->begin(), count, el);
    }


    /** Inserts copies of the elements from the specified [beginIt ... endIt)
        iterator range at the beginning of the collection.
        endIt points to the location just *after* the last element to add.

        The beginIt and endIt iterators must NOT refer to the target collection that is being changed. They can be from an arbitrary other
        collection -- also from a collection of a different type. The only condition is that the element type of the
        source collection must be compatible to the element type of the target collection.
        
        Returns an iterator to first inserted element. If no elements are inserted (beginIt == endIt)
        then \c begin() is returned.
        */
    template< class InputIt >
    Iterator insertSequenceAtBegin( InputIt beginIt, InputIt endIt )
    {
        return BaseCollectionType:: template insert<InputIt>( this->begin(), beginIt, endIt);
    }



    /** Inserts the elements from the specified initializer list at the beginning of the collection.
    
        This can be used to insert multiple elements with the {...} notation. For example:

        \begin
        Array<int> ar;

        ar.insertSequenceAtBegin( {1, 4, 7} );    // inserts three elements at the start of the array

        */
    Iterator insertSequenceAtBegin( std::initializer_list<Element> initList )
    {
        return BaseCollectionType::insert( this->begin(), initList);
    }


    /** Constructs a new element and inserts it at the beginning of the collection.

        Same as insertNewAt( begin(), args...).    
    */
    template< class... Args > 
    Iterator insertNewAtBegin(Args&&... args)
    {
        return insertNewAt( this->begin(), std::forward<Args>(args)... );
    }


    
    /** Removes the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    void removeLast()
    {
        if( this->isEmpty())
    		throw OutOfRangeError("StdPositionalCollection::removeLast called on empty array.");

        this->pop_back();
    }

    /** Removes the first element from the collection. Throws OutOfRangeError if the collection
        is empty.*/
    void removeFirst()
    {
        if( this->isEmpty())
    		throw OutOfRangeError("StdPositionalCollection::removeFirst called on empty array.");

        BaseCollectionType::erase( BaseCollectionType::begin() );
    }


    
    

    /** Changes the size of the collection to \c count number of elements.
    
        If the collection is currently bigger then the elements at the end are removed.

        If the collection is currently smaller then new default-constructed elements are added
        at the end. If you the new elements to be initialized differently then you should use
        the setSize(Size, const Element&) overload of this method instead.
    */
    void setSize( Size count )
    {
        BaseCollectionType::resize(count);
    }


    /** Changes the size of the collection to \c count number of elements.
    
        If the collection is currently bigger then the elements at the end are removed.

        If the collection is currently smaller then copies of \c padValue are added at the end.
    */
    void setSize( Size count, const Element& padValue )
    {
        BaseCollectionType::resize(count, padValue);
    }


	
    class ElementMatcher_
    {
    public:
        ElementMatcher_(const Element& element)
            : _element( element )
        {
        }

		// this is a template function so that it works with both normal and const iterators
		// and set references
		template<class CollType, class IteratorType>
		void operator() (CollType& coll, IteratorType& it)
        {
			it = std::find( it, coll.end(), _element);
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


		// this is a template function so that it works with both normal and const iterators
		// and set references
		template<class CollType, class IteratorType>
		void operator() (CollType& coll, IteratorType& it)
        {
            // note that the "it" parameter is NEVER equal to end() when we are called.
            // That also means that we are never called for empty maps.

            while( ! _matchFunc(it) )
            {
                ++it;
                if( it==coll.end() )
                    break;
            }
        }   


	private:
		MatchFuncType _matchFunc;
    };
	
	using ElementFinder = SequenceFilter< StdPositionalCollection, ElementMatcher_>;
	using ConstElementFinder = SequenceFilter< const StdPositionalCollection, ElementMatcher_>;

	template<typename MatchFuncType>
	using CustomFinder = SequenceFilter< StdPositionalCollection, FuncMatcher_<MatchFuncType> >;

	template<typename MatchFuncType>
	using ConstCustomFinder = SequenceFilter< const StdPositionalCollection, FuncMatcher_<MatchFuncType> >;

	/** Searches for the specified element in the collection and returns a \ref finder.md "finder object"
		with the results.		
		*/
	ElementFinder findAll(const Element& elToFind)
	{
        return ElementFinder(*this, ElementMatcher_(elToFind) );
	}

	/** Searches for the specified element in the collection and returns a \ref finder.md "finder object"
		with the results.		
		*/
	ConstElementFinder findAll(const Element& elToFind) const
	{
        return ConstElementFinder(*this, ElementMatcher_(elToFind) );
	}


	/** Searches for all elements for which the specified match function returns true.
		
		The match function can be any function that takes a collection iterator as its parameter
		and returns true if the element at the corresponding position should be in the find results.

		findAllCustom returns a \ref finder.md "finder object" with the results.
		*/
	template<class MatchFuncType>
	CustomFinder<MatchFuncType> findAllCustom( MatchFuncType matchFunction )
	{
		return CustomFinder<MatchFuncType>(*this, FuncMatcher_<MatchFuncType>(matchFunction) );
	}

	/** Searches for all elements for which the specified match function returns true.
		
		The match function can be any function that takes a collection iterator as its parameter
		and returns true if the element at the corresponding position should be in the find results.

		findAllCustom returns a \ref finder.md "finder object" with the results.
		*/
	template<class MatchFuncType>
	ConstCustomFinder<MatchFuncType> findAllCustom( MatchFuncType matchFunction ) const
	{
		return ConstCustomFinder<MatchFuncType>(*this, FuncMatcher_<MatchFuncType>(matchFunction) );
	}


    

    /** Searches for the first element that compares equal to the specified \c el parameter.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    Iterator find( const Element& el )
    {
        return std::find( this->begin(), this->end(), el );
    }

    /** Searches for the first element that compares equal to the specified \c el parameter,
        starting from the position indicated by \c startPos.

        The first possible hit is at \c startPos.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    Iterator find( const Element& el, Iterator startPos )
    {
        return std::find( startPos, this->end(), el );
    }


    /** Const version of find() - returns a read-only iterator.
    */
    ConstIterator find( const Element& el ) const
    {
        return std::find( this->begin(), this->end(), el );
    }    

    /** Searches for the first element that compares equal to the specified \c el parameter,
        starting from the position indicated by \c startPos.

        The first possible hit is at \c startPos.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    ConstIterator find( const Element& el, ConstIterator startPos ) const
    {
        return std::find( startPos, this->end(), el );
    }


    /** Searches for the first element for which the specified match function returns true.        
        
		The match function can be any function that takes a collection iterator as its parameter
		and returns true if the element at the corresponding position should be in the find results.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    template<typename MatchFuncType>
	Iterator findCustom( MatchFuncType matchFunc )
	{
		auto it = this->begin();
		while( it != this->end() )
		{
			if( matchFunc(it) )
				break;

			++it;
		}

		return it;
	}


    /** Const version of findCustom() - returns a read-only iterator.
    */
    template<typename MatchFuncType>
	ConstIterator findCustom( MatchFuncType matchFunc ) const
	{
		auto it = this->begin();
		while( it != this->end() )
		{
			if( matchFunc(it) )
				break;

			++it;
		}

		return it;
	}


	/** Returns true if the collection contains the specified element.	
    */
    bool contains( const Element& el ) const
    {
        return find(el) != end();
    }


    /** Searches for the last element that compares equal to the specified \c el parameter.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    Iterator reverseFind( const Element& el )
    {
        Iterator it = end();
        Iterator beginIt = begin();

        while( it!=beginIt )
        {
            --it;

            if( *it == el)
                return it;
        }

        return end();
    }

    /** Searches backwards from the specified start position for
        the last element that compares equal to the specified \c el parameter,

        The first possible hit is at \c startPos.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    Iterator reverseFind( const Element& el, Iterator startPos )
    {
        Iterator it = startPos;
        if( it!=end() )
            ++it;
        Iterator beginIt = begin();

        while( it!=beginIt )
        {
            --it;

            if( *it == el)
                return it;
        }

        return end();
    }


    /** Const version of reverseFind() - returns a read-only iterator.
    */
    ConstIterator reverseFind( const Element& el ) const
    {
        ConstIterator it = end();
        ConstIterator beginIt = begin();

        while( it!=beginIt )
        {
            --it;

            if( *it == el)
                return it;
        }

        return end();
    }    

    /** Const version of reverseFind() - returns a read-only iterator.
    */
    ConstIterator reverseFind( const Element& el, ConstIterator startPos ) const
    {
        ConstIterator it = startPos;
        if( it!=end() )
            ++it;
        ConstIterator beginIt = begin();

        while( it!=beginIt )
        {
            --it;

            if( *it == el)
                return it;
        }

        return end();
    }    


    /** Searches for the last element for which the specified match function returns true.        
        
		The match function can be any function that takes a collection iterator as its parameter
		and returns true if the element at the corresponding position should be in the find results.
    
        Returns an iterator to the found element, or end() if no such element is found.    
    */
    template<typename MatchFuncType>
	Iterator reverseFindCustom(MatchFuncType matchFunc )
	{
		auto it = this->end();
		while( it != this->begin() )
		{
			--it;

			if( matchFunc(it) )
				return it;
		}

		return this->end();
	}


    /** Const version of reverseFindCustom() - returns a read-only iterator.
    */
    template<typename MatchFuncType>
	ConstIterator reverseFindCustom(MatchFuncType matchFunc ) const
	{
		auto it = this->end();
		while( it != this->begin() )
		{
			--it;

			if( matchFunc(it) )
				return it;
		}

		return this->end();
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
		return BaseCollectionType::rbegin();
	}


    /** Const version of reverseBegin() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator reverseBegin() const noexcept
	{
		return BaseCollectionType::rbegin();
	}


    /** Const version of reverseBegin() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator constReverseBegin() const noexcept
	{
		return BaseCollectionType::crbegin();
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
		return BaseCollectionType::rend();
	}


    /** Const version of reverseEnd() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator reverseEnd() const noexcept
	{
		return BaseCollectionType::rend();
	}


    /** Const version of reverseEnd() - returns an iterator for read-only access to the elements in reversed order.
    */
    ConstReverseIterator constReverseEnd() const noexcept
	{
		return BaseCollectionType::crend();
	}


};


}


#endif
