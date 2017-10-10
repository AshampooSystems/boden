#ifndef BDN_StdCollection_H_
#define BDN_StdCollection_H_

#include <bdn/sort.h>

namespace bdn
{

template<class BaseCollectionType >
class StdCollection : public BaseCollectionType, public Base
{
public:

    /** The type of the collection elements.*/
    typedef typename BaseCollectionType::value_type Element;

    /** The type of the allocator that is used to manage the collection's memory allocations.*/
    typedef typename BaseCollectionType::allocator_type Allocator;

    /** The integer type that is used to represent indexes and collection sizes.
        This is usually an alias to size_t.*/
    typedef typename BaseCollectionType::size_type Size;


    /** The class of iterator objects for read/write access to elements.*/
    typedef typename BaseCollectionType::iterator Iterator;

    /** The class of iterator objects for read-only access to elements.*/
    typedef typename BaseCollectionType::const_iterator ConstIterator;


    /** The class of iterator objects for iteration over the collection elements in reverse order (with read/write access to the elements).*/
    typedef typename BaseCollectionType::reverse_iterator ReverseIterator;

    /** The class of iterator objects for iteration over the collection elements in reverse order (with read-only access to the elements).*/
    typedef typename BaseCollectionType::const_reverse_iterator ConstReverseIterator;


    template<class... Args>
    StdCollection(Args... args)
     : BaseCollectionType( std::forward<Args>(args)... )
    {
    }


    /** Returns a copy of the allocator object that manages the collection's memory.*/
    Allocator getAllocator() const
    {
        return BaseCollectionType::get_allocator();
    }


       
    /** Returns an iterator to the "first" element of the collection.

        An iterator is similar to a pointer. It refers to an element or a position in the collection.
        The iterator can be advanced to the next element with the ++ operator. For many collection types
        the -- operator can be used to go back to the previous element.
        The element the iterator points to can be accessed with the * operator. If the collection elements
        are objects the then iterator's -> operator can be used to directly access method and fields of the element
        it points to.

        Example:

        \code

        Array<String> ar = {"one", "two", "three"};

        // get an iterator to the first element
        Array<String>::Iterator it = ar.begin();

        // note that the previous line could also be written with the "auto"
        // keyword instead of the iterator type:
        // auto it = ar.begin().        

        // access the first element
        String first = *it;

        // advance to the next element
        ++it

        // directly call a method of the second element
        size_t secondLength = it->getLength();

        // go back to the first element (not supported by all collections / iterator types).
        --it;

        \endcode
        
        When the iterator is moved to other elements then the ordering of the elements depends on the
        actual underlying collection type. 

        Iterators can be compared with each other with the == and != operators.
        They are "equal" when they point to the same element / position in the collection.

        If the collection is empty then calling begin() is still ok. It will return the same iterator as end().
    */
    Iterator begin() noexcept
    {
        return BaseCollectionType::begin();
    }

    /** Const version of begin() - returns an iterator for read-only access to the elements.
    */
    ConstIterator begin() const noexcept
    {
        return BaseCollectionType::begin();
    }


    /** Const version of begin() - returns an iterator for read-only access to the elements.
    */
    ConstIterator constBegin() const noexcept
    {
        return BaseCollectionType::cbegin();
    }


    /** Returns an iterator that points to the point just after (!) the last element in the collection.
        
        The end iterator does point to a valid element. It is intended as a marker during iterations
        to know when to stop.

        For example, to iterate over all elements of the collection one could use the following code:

        \code

        Array<int> ar = {1, 5, 7};

        auto it = ar.begin();
        while( it != ar.end() )
        {
            // do something with the element
            ...

            // go to next element
            ++it;
        }

        \endcode

    */
    Iterator end() noexcept
    {
        return BaseCollectionType::end();
    }

    /** Const version of end() - returns an iterator for read-only access to the elements.
    */
    ConstIterator end() const noexcept
    {
        return BaseCollectionType::end();
    }


    /** Const version of end() - returns an iterator for read-only access to the elements.
    */
    ConstIterator constEnd() const noexcept
    {
        return BaseCollectionType::cend();
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



    /** Returns true if the collection is empty*/
    bool isEmpty() const noexcept
    {
        return BaseCollectionType::empty();
    }


    /** Returns the number of elements in the collection.*/
    Size getSize() const noexcept
    {
        return BaseCollectionType::size();
    }


    /** Returns the maximum number of elements the collection can potentially have.
        This refers to the theoretical maximum, provided that enough memory is available.*/
    Size getMaxSize() const noexcept
    {
        return BaseCollectionType::max_size();
    }
    

    /** Removes all elements from the collection.*/
    void clear() noexcept
    {
        BaseCollectionType::clear();
    }
    




    /** Removes the element at the specified position.
    
        Returns an iterator to the following element. If the removed
        element was at the end of the collection then the end() iterator is returned.
    */
    Iterator removeAt( ConstIterator pos )
    {
        return BaseCollectionType::erase(pos);
    }


    /** Removes all elements in the iterator range [beginIt ... endIt).
        
        endIt indicates to the position just after the last element to remove.

        Returns an iterator to element that follows the removed range. If the removed
        range was at the end of the collection then the end() iterator is returned.
    */
    Iterator removeSection( ConstIterator beginIt, ConstIterator endIt )
    {
        return BaseCollectionType::erase(beginIt, endIt);
    }




};


}


#endif
