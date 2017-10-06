#ifndef BDN_StdSequenceCollection_H_
#define BDN_StdSequenceCollection_H_

#include <bdn/StdCollection.h>

namespace bdn
{


/** Helper for collections that organize their elements in a user-specified sequence (like arrays, lists, etc.).
*/
template<class BaseCollectionType >
class StdSequenceCollection : public StdCollection< BaseCollectionType >
{
public:
    
    using typename StdCollection< BaseCollectionType >::Element;
    using typename StdCollection< BaseCollectionType >::Size;
    using typename StdCollection< BaseCollectionType >::Iterator;
    using typename StdCollection< BaseCollectionType >::ConstIterator;
    using typename StdCollection< BaseCollectionType >::ReverseIterator;
    using typename StdCollection< BaseCollectionType >::ConstReverseIterator;
    
    template<class... Args>
    StdSequenceCollection(Args... args)
     : StdCollection<BaseCollectionType>( std::forward<Args>(args)... )
    {
    }

    
    /** Returns a reference to the first element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    Element& getFirst()
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdSequenceCollection::firstElement called on empty array.");

        return BaseCollectionType::front();
    }

    /** Returns a const reference to the first element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    const Element& getFirst() const
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdSequenceCollection::firstElement called on empty array.");

        return BaseCollectionType::front();
    }



    /** Returns a reference to the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    Element& getLast()
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdSequenceCollection::lastElement called on empty array.");

        return BaseCollectionType::back();
    }


    /** Returns a const reference to the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    const Element& getLast() const
    {
        if( this->isEmpty() )
    		throw OutOfRangeError("StdSequenceCollection::lastElement called on empty array.");

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
    		throw OutOfRangeError("StdSequenceCollection::removeLast called on empty array.");

        this->pop_back();
    }

    /** Removes the first element from the collection. Throws OutOfRangeError if the collection
        is empty.*/
    void removeFirst()
    {
        if( this->isEmpty())
    		throw OutOfRangeError("StdSequenceCollection::removeFirst called on empty array.");

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


    /** Searches for the first element for which the specified condition function returns true.        
        conditionFunc must take a collection element reference as its only parameter and return a boolean.
    
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


    /** Searches for the last element for which the specified condition function returns true.        
        conditionFunc must take a collection element reference as its only parameter and return a boolean.
    
        Returns an iterator to the found element, or end() if no such element is found.    
    */
    template<typename ConditionFuncType>
	Iterator reverseFindCondition(ConditionFuncType conditionFunc )
	{
        ReverseIterator endIt = this->reverseEnd();
        ReverseIterator it = std::find_if( this->reverseBegin(), endIt, conditionFunc );
        if(it==endIt)
            return this->end();

        // it.base is the iterator of the element AFTER the one it points to
        return --it.base();
	}


    /** Const version of reverseFindCondition() - returns a read-only iterator.
    */
    template<typename ConditionFuncType>
	ConstIterator reverseFindCondition(ConditionFuncType conditionFunc ) const
	{
        ConstReverseIterator endIt = this->reverseEnd();
        ConstReverseIterator it = std::find_if( this->reverseBegin(), endIt, conditionFunc );
        if(it==endIt)
            return this->end();

        // it.base is the iterator of the element AFTER the one it points to
        return --it.base();
	}
    

    /** Sorts the elements from small to big (using the element's < operator
        to compare them).

        When elements compare equal to each other (i.e. neither one
        is smaller than the other) then they will obviously end up next to each other.
        However, their relative ordering amongst themselves is undefined. See stableSort() for an alternative.
        */
    void sort()
    {
        std::sort( this->begin(), this->end() );
    }


    /** Sorts the elements in a custom order. comesBefore must be a function that
        takes references to two elements as its parameters and returns true if the first one
        should come before the second one.
        
        When elements compare equal to each other (i.e. neither one
        is smaller than the other) then they will obviously end up next to each other.
        However, their relative ordering amongst themselves is undefined. See stableSort() for an alternative.
        */
    template<class ComesBeforeFuncType>
    void sort(ComesBeforeFuncType comesBefore )
    {
        std::sort( this->begin(), this->end(), comesBefore );
    }


    /** Like sort() except that the ordering is stable. That means that 
        when elements compare equal to each other (i.e. neither one
        is smaller than the other) then they remain in their original relative order
        amongst themselves.
    */
    void stableSort()
    {
        std::stable_sort( this->begin(), this->end() );
    }


    /** Like sort() except that the ordering is stable.
        
        Sorts the elements in a custom order. comesBefore must be a function that
        takes references to two elements as its parameters and returns true if the first one
        should come before the second one.

        When elements compare equal to each other (i.e. neither one
        is smaller than the other) then they remain in their original relative order
        amongst themselves.
        */
    template<class ComesBeforeFuncType>
    void stableSort( ComesBeforeFuncType comesBeforeFunc )
    {
        std::stable_sort( this->begin(), this->end(), comesBeforeFunc );
    }
};


}


#endif
