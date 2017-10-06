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
    
    template<class... Args>
    StdSequenceCollection(Args... args)
     : StdCollection( std::forward<Args>(args)... )
    {
    }

    
    /** Returns a reference to the first element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    typename StdCollection::ElementType& getFirst()
    {
        if(isEmpty())
    		throw OutOfRangeError("StdSequenceCollection::firstElement called on empty array.");

        return BaseCollectionType::front();
    }

    /** Returns a const reference to the first element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    const typename StdCollection::ElementType& getFirst() const
    {
        if(isEmpty())
    		throw OutOfRangeError("StdSequenceCollection::firstElement called on empty array.");

        return BaseCollectionType::front();
    }



    /** Returns a reference to the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    typename StdCollection::ElementType& getLast()
    {
        if(isEmpty())
    		throw OutOfRangeError("StdSequenceCollection::lastElement called on empty array.");

        return BaseCollectionType::back();
    }


    /** Returns a const reference to the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    const typename StdCollection::ElementType& getLast() const
    {
        if(isEmpty())
    		throw OutOfRangeError("StdSequenceCollection::lastElement called on empty array.");

        return BaseCollectionType::back();
    }



    
    
    /** Adds a new element to the collection (at the end).
    */
    void add( const ElementType& value )
    {
        BaseCollectionType::push_back(value);
    }


    /** Like add(), but instead of the new element being a copy of the specified
        element, the C++ move semantics are used to move the element data from the parameter
        \c el to the new collection element.
        */
    void add( ElementType&& value )
    {
        BaseCollectionType::push_back( std::forward<ElementType>(value) );
    }


    /** Adds \c count copies copy of the specified element \c el to the collection (at the end).        
    */
    void addMultipleCopies( SizeType count, const ElementType& el)
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
        BaseCollectionType::insert( end(), beginIt, endIt);
    }


    /** Adds the elements from the specified initializer list to the collection (at the end).

        This can be used to add multiple elements with the {...} notation. For example:

        \begin
        Array<int> ar;

        ar.addSequence( {1, 4, 7} );    // adds three elements to the array

        */
    void addSequence( std::initializer_list<ElementType> initList )
    {
        BaseCollectionType::insert( end(), initList );
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
    ElementType& addNew( Args&&... args )
    {
        BaseCollectionType::emplace_back( std::forward<Args>(args)... );
        return back();
    }



    
    /** Inserts a copy of the specified element \c el at position \c pos.
        
        If \c pos points to an element of the collection then the new element is inserted
        before that element.

        If \c pos points to the collection end then the new element is added at the end.

        Returns an iterator to the newly inserted element.                
    */
    Iterator insertAt( ConstIterator pos, const ElementType& el)
    {
        return BaseCollectionType::insert(pos, el);
    }


    /** Like insertAt() but instead of being a copy the newly inserted element is created
        with C++ move semantics from the \c el argument.        
    */
    Iterator insertAt( ConstIterator pos, ElementType&& el)
    {
        return BaseCollectionType::insert(pos, std::forward<ElementType>(el) );
    }


    /** Inserts \c count copies copy of the specified element \c el at the specified position \pos.
        
        If \c pos points to an element of the collection then the new elements are inserted
        before that element.

        If \c pos points to the collection end then the new elements are added at the end.

        Returns an iterator to first inserted element. If count is 0 then \c pos is returned.
    */
    Iterator insertMultipleCopiesAt( ConstIterator pos, SizeType count, const ElementType& el)
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
        return BaseCollectionType::insert<InputIt>(pos, beginIt, endIt);
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
    Iterator insertSequenceAt( ConstIterator pos, std::initializer_list<ElementType> initList )
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
    void insertAtBegin( const ElementType& el )
    {
        BaseCollectionType::insert( begin(), el);
    }

    /** Inserts an element at the beginning of the collection.

        Same as insert( begin(), el).    
    */
    void insertAtBegin( ElementType&& el )
    {
        BaseCollectionType::insert( begin(), std::forward<ElementType>(el) );
    }


    
    /** Inserts \c count copies copy of the specified element \c el at the beginning of the collection.
        
        Returns an iterator to first inserted element. If count is 0 then \c begin() is returned.
    */
    Iterator insertMultipleCopiesAtBegin( SizeType count, const ElementType& el)
    {
        return BaseCollectionType::insert( BaseCollectionType::begin(), count, el);
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
        return BaseCollectionType::insert<InputIt>( BaseCollectionType::begin(), beginIt, endIt);
    }



    /** Inserts the elements from the specified initializer list at the beginning of the collection.
    
        This can be used to insert multiple elements with the {...} notation. For example:

        \begin
        Array<int> ar;

        ar.insertSequenceAtBegin( {1, 4, 7} );    // inserts three elements at the start of the array

        */
    Iterator insertSequenceAtBegin( std::initializer_list<ElementType> initList )
    {
        return BaseCollectionType::insert( BaseCollectionType::begin(), initList);
    }


    /** Constructs a new element and inserts it at the beginning of the collection.

        Same as insertNewAt( begin(), args...).    
    */
    template< class... Args > 
    Iterator insertNewAtBegin(Args&&... args)
    {
        return insertNewAt( begin(), std::forward<Args>(args)... );
    }


    
    /** Removes the last element of the collection. Throws OutOfRangeError if the collection
        is empty.*/
    void removeLast()
    {
        if(isEmpty())
    		throw OutOfRangeError("StdSequenceCollection::removeLast called on empty array.");

        BaseCollectionType::pop_back();
    }

    /** Removes the first element from the collection. Throws OutOfRangeError if the collection
        is empty.*/
    void removeFirst()
    {
        if(isEmpty())
    		throw OutOfRangeError("StdSequenceCollection::removeFirst called on empty array.");

        BaseCollectionType::erase( BaseCollectionType::begin() );
    }
    

    /** Changes the size of the collection to \c count number of elements.
    
        If the collection is currently bigger then the elements at the end are removed.

        If the collection is currently smaller then new default-constructed elements are added
        at the end. If you the new elements to be initialized differently then you should use
        the setSize(SizeType, const ElementType&) overload of this method instead.
    */
    void setSize( SizeType count )
    {
        BaseCollectionType::resize(count);
    }


    /** Changes the size of the collection to \c count number of elements.
    
        If the collection is currently bigger then the elements at the end are removed.

        If the collection is currently smaller then copies of \c padValue are added at the end.
    */
    void setSize( SizeType count, const ElementType& padValue )
    {
        BaseCollectionType::resize(count, padValue);
    }
    

    /** Searches for the first element that compares equal to the specified \c el parameter.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    Iterator find( const ElementType& el )
    {
        return std::find(begin(), end(), el );
    }


    /** Const version of find() - returns a read-only iterator.
    */
    ConstIterator find( const ElementType& el ) const
    {
        return std::find(begin(), end(), el );
    }    


    /** Searches for the first element for which the specified condition function returns true.        
        conditionFunc must take a collection element reference as its only parameter and return a boolean.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    template<class ConditionFuncType>
	Iterator findCondition(ConditionFuncType conditionFunc )
	{
        return std::find_if( begin(), end(), conditionFunc );
	}


    /** Const version of findCondition() - returns a read-only iterator.
    */
    template<class ConditionFuncType>
	ConstIterator findCondition(ConditionFuncType conditionFunc ) const
	{
        return std::find_if( begin(), end(), conditionFunc );
	}


    /** Searches for the last element that compares equal to the specified \c el parameter.
    
        Returns an iterator to the found element, or end() if no such element is found.
    */
    Iterator reverseFind( const ElementType& el )
    {
        ReverseIterator endIt = reverseEnd();
        ReverseIterator it = std::find(reverseBegin(), endIt, el );
        if(it==endIt)
            return end();

        // it.base is the iterator of the element AFTER the one it points to
        return --it.base();
    }


    /** Const version of reverseFind() - returns a read-only iterator.
    */
    ConstIterator reverseFind( const ElementType& el ) const
    {
        ConstReverseIterator endIt = reverseEnd();
        ConstReverseIterator it = std::find(reverseBegin(), endIt, el );
        if(it==endIt)
            return end();

        // it.base is the iterator of the element AFTER the one it points to
        return --it.base();
    }    


    /** Searches for the last element for which the specified condition function returns true.        
        conditionFunc must take a collection element reference as its only parameter and return a boolean.
    
        Returns an iterator to the found element, or end() if no such element is found.    
    */
    template<typename ConditionFuncType>
	Iterator reverseFindCondition(ConditionFuncType conditionFunc )
	{
        ReverseIterator endIt = reverseEnd();
        ReverseIterator it = std::find_if(reverseBegin(), endIt, conditionFunc );
        if(it==endIt)
            return end();

        // it.base is the iterator of the element AFTER the one it points to
        return --it.base();
	}


    /** Const version of reverseFindCondition() - returns a read-only iterator.
    */
    template<typename ConditionFuncType>
	ConstIterator reverseFindCondition(ConditionFuncType conditionFunc ) const
	{
        ConstReverseIterator endIt = reverseEnd();
        ConstReverseIterator it = std::find_if(reverseBegin(), endIt, conditionFunc );
        if(it==endIt)
            return end();

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
        std::sort( begin(), end() );
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
        std::sort( begin(), end(), comesBefore );
    }


    /** Like sort() except that the ordering is stable. That means that 
        when elements compare equal to each other (i.e. neither one
        is smaller than the other) then they remain in their original relative order
        amongst themselves.
    */
    void stableSort()
    {
        std::stable_sort( begin(), end() );
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
    void stableSort( ComesBeforeFuncType comesBefore )
    {
        std::stable_sort( begin(), end(), comesBefore );
    }

};


}


#endif
