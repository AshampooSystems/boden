#ifndef BDN_List_H_
#define BDN_List_H_

#include <list>

#include <bdn/StdSequenceCollection.h>

namespace bdn
{

/** Implements a double linked list with elements of the type indicated by the first template
    parameter. For example List<String> is an array of Strings.
    
    The List class is derived from std::list and is 100 % compatible with it.
    It can be used as a drop-in replacement.

    List provides renamed aliases for some std::list methods and types to ensure that
    they match the naming conventions used otherwise in the framework. The new
    names are also intended to make using the class more intuitive for newcomers that may not be familiar with
    the C++ standard library.

    Note that List is also derived from bdn::Base, so it can be used with smart pointers (see bdn::P).
    
    Additionally, some utility and convenience functions have been added to List that std::list
    does not have to make commonly used functionality more convenient (like sorting, etc).

    Like the C++ standard library collections, one can specify an "allocator" type as an optional
    second template argument. This is used when special custom memory management is needed.
    If you do not need that then you should ignore the ALLOCATOR template argument and leave it at the default. 

*/
template<typename ELTYPE, class ALLOCATOR = std::allocator<ELTYPE> >
class List : public StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >
{
public:
    
    using typename StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::Element;
    using typename StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::Size;
    using typename StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::Iterator;
    using typename StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::ConstIterator;
    using typename StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::ReverseIterator;
    using typename StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::ConstReverseIterator;
    
    /** Creates an empty array.*/
    List() noexcept( noexcept(ALLOCATOR()) )
     : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >( ALLOCATOR() )
    {
    }

    /** Constructs an array that uses a specific allocator object.*/
    explicit List( const ALLOCATOR& alloc ) noexcept
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >(alloc)
    {
    }


    /** Initializes the array with \c count copies of \c el. 
    
        Optionally, one can also pass an allocator object for custom memory management.
    */
    List( Size count, const Element& el, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >(count, el, alloc)
    {
    }


    /** Initializes the array with \c count default-constructed elements. 
    
        Optionally, one can also pass an allocator object for custom memory management.
    */
    explicit List( Size count )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >(count)
    {
    }
    

    /** Initializes the array with copies of the elements from the iterator range [beginIt ... endIt) 

        Optionally, one can also pass an allocator object for custom memory management.
    */
    template< class InputIt >
    List( InputIt beginIt, InputIt endIt, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >(beginIt, endIt, alloc)
    {
    }

    /** Initializes the List with copies of the elements from the specified other array.*/
    List( const List& other )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >( static_cast<const std::list<ELTYPE, ALLOCATOR>&>(other) )
    {
    }


    /** Initializes the List with copies of the elements from the specified std::list object.*/
    List( const std::list<ELTYPE, ALLOCATOR>& other )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >(other)
    {
    }


    /** Initializes the List with copies of the elements from the specified std::list or List object.
        
        The specified allocator object is used to initialize the array's internal allocator.
        */
    List( const std::list<ELTYPE, ALLOCATOR>& other, const ALLOCATOR& alloc )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >(other, alloc)
    {
    }

    /** Moves the data from the specified other array to this array. The other array is invalidated by this.*/
    List( List&& other ) noexcept
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >( std::move( static_cast<std::list<ELTYPE, ALLOCATOR>&&>(other) ) )
    {
    }


    /** Moves the data from the specified other vector to this array. The other array is invalidated by this.*/
    List( std::list<ELTYPE, ALLOCATOR>&& other ) noexcept
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >( std::move(other) )
    {
    }


    /** Moves the data from the specified other array to this array. The other array is invalidated by this.
    
        The specified allocator object is used to initialize the array's internal allocator.
        */
    List( List&& other, const ALLOCATOR& alloc )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >( std::forward(other), alloc )
    {
    }


    /** Moves the data from the specified other vector to this array. The other array is invalidated by this.
    
        The specified allocator object is used to initialize the array's internal allocator.*/
    List( std::list<ELTYPE, ALLOCATOR>&& other, const ALLOCATOR& alloc )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >( std::forward(other), alloc )
    {
    }


    /** Initializes the array with the specified initializer list. This is called by the compiler
        when {...} initialization is used (see example below).

        Optionally, one can also pass an allocator object for custom memory management.

        Example:

        \code

        // initialize the array with these elements: 1, 13, 42
        List<int> ar { 1, 13, 42 };

        \endcode
    */
    List( std::initializer_list<Element> initList, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >( initList, alloc )
    {
    }


    /** Replaces the current contents of the array with copies of the elements from the specified other
        List.
        
        Returns a reference to this List object.
        */
    List& operator=( const List& other )
    {
        std::list<ELTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


    /** Replaces the current contents of the array with copies of the elements from the specified
        vector.
        
        Returns a reference to this List object.
        */
    List& operator=( const std::list<ELTYPE, ALLOCATOR>& other )
    {
        std::list<ELTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


     /** Replaces the current contents of the array with copies of the elements from the specified
        initializer list. This is called by the compiler if a  "= {...} " statement is used. For example:

        \code
        List<int> ar;

        ar = {1, 17, 42};
        \endcode

        Returns a reference to this List object.
        */
    List& operator=( std::initializer_list<Element> initList )
    {
        std::list<ELTYPE, ALLOCATOR>::operator=(initList);
        return *this;
    }  




    /** Moves the data from the specified other List object to this array, replacing any current contents in the process.
        The other List object is invalidated by this operation.
        */
    List& operator=( List&& other )
        noexcept( ALLOCATOR::propagate_on_container_move_assignment::value || ALLOCATOR::is_always_equal::value )
    {
        std::list<ELTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }
    

    /** Moves the data from the specified other List object to this array, replacing any current contents in the process.
        The other List object is invalidated by this operation.
        */
    List& operator=( std::list<ELTYPE, ALLOCATOR>&& other )
        noexcept( ALLOCATOR::propagate_on_container_move_assignment::value || ALLOCATOR::is_always_equal::value )
    {
        std::list<ELTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }

    
     /** Sorts the elements in ascending order (small first), using the element's < operator
        to compare them. 

        If you need descending order or a custom order, there is also a variant of sort
        that takes a comparison function as a parameter.

        Examples:

        \code

        bool myComparisonFunc(const String& a, const String& b)
        {
            // return true if a should come before b
            ...
        }

        void doSort()
        {
            List<String> myList = ...;

            // sort in ascending order
            myList.sort(); 

            // sort in descending order. bdn::descending() is a predefined
            // comparison function provided by the boden framework.
            myList.sort(descending);

            // sort in a custom order, defined by the custom myComparisonFunc.
            myList.sort( myComparisonFunc );
        }

        \endcode

        Stability
        ---------

        sort is *not* guaranteed to be implemented with a stable sorting algorithm.
        Note that "stability" is a special term when discussing sorting algorithms.
        It refers to how elements that are "equal" to each other (neither one of them is
        smaller than the other) are handled. Stable sorting algorithms guarantee that
        equal elements keep their original relative order amongst each other. Non-stable
        sorting algorithms do not guarantee that.

        If you need a stable sorting algorithm use stableSort() instead.
        
        */
    void sort()
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::sort();
    }


    /** Sorts the elements in a custom order. comesBefore must be a function that
        takes references to two elements as its parameters and returns true if the first one
        should come before the second one. There are two predefined comparison functions
        that can be used: pass bdn::ascending<ElementTypeName> to get ascending order or
        bdn::descending<ElementTypeName> to get descending order.
        
        Examples:

        \code

        bool myComparisonFunc(const String& a, const String& b)
        {
            // return true if a should come before b
            ...
        }

        void doSort()
        {
            Array<String> myArray = ...;

            // sort in ascending order
            myArray.sort(); 

            // sort in descending order. bdn::descending() is a predefined
            // comparison function provided by the boden framework.
            myArray.sort( descending );

            // sort in a custom order, defined by the custom myComparisonFunc.
            myArray.sort( myComparisonFunc );
        }

        \endcode

        Stability
        ---------

        sort is *not* guaranteed to be implemented with a stable sorting algorithm.
        Note that "stability" is a special term when discussing sorting algorithms.
        It refers to how elements that are "equal" to each other (neither one of them is
        smaller than the other) are handled. Stable sorting algorithms guarantee that
        equal elements keep their original relative order amongst each other. Non-stable
        sorting algorithms do not guarantee that.

        If you need a stable sorting algorithm use stableSort() instead.        
        */
    template<class ComesBeforeFuncType>
    void sort(ComesBeforeFuncType comesBefore )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::sort(comesBefore);
    }


    /** Like sort() except that the sorting algorithm is guaranteed to be stable. See sort() documentation for
        more information about what this means.
    */
    void stableSort()
    {
        // std::list::sort is always stable.
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::sort();
    }


    /** Like sort() except that the sorting algorithm is guaranteed to be stable. See sort() documentation for
        more information about what this means.
    */
    template<class ComesBeforeFuncType>
    void stableSort( ComesBeforeFuncType comesBeforeFunc )
    {
        // std::list::sort is always stable.
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::sort(comesBeforeFunc);
    }
    


    
    /** In general, this collection operation prepares the collection for a bigger insert operation.
        This function is provided for consistency with other collection types only - for bdn::List this
        function has no effect and does nothing.
    */
    void prepareForSize(Size size)
    {
        // do nothing
    }



    /** Merges two sorted lists.
        Both lists *must* be sorted in ascending order (as defined by the list element's < operator)
        before this is called. If this is not the case then undefined behaviour, including crashes, might occur.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.
        
        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        
        */
    void stealAllAndMergeSorted( List& other )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge(other);
    }


    /** Merges two sorted lists.
        Both lists *must* be sorted in ascending order (as defined by the list element's < operator)
        before this is called. If this is not the case then undefined behaviour, including crashes, might occur.
        All elements from the other list are transferred to this list and inserts them in the correct
        places so that the result is a properly sorted list again.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.
        
        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        

        */
    void stealAllAndMergeSorted( std::list<ELTYPE, ALLOCATOR>& other )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge(other);
    }


    /** Merges two sorted lists.
        Both lists *must* be sorted in ascending order (as defined by the list element's < operator)
        before this is called.  If this is not the case then undefined behaviour, including crashes, might occur.
        All elements from the other list are transferred to this list and inserts them in the correct
        places so that the result is a properly sorted list again.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.

        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        
        
        */
    void stealAllAndMergeSorted( List&& other )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge( std::move( static_cast<std::list<ELTYPE, ALLOCATOR>&&>(other) ) );
    }


    /** Merges two sorted lists.
        Both lists *must* be sorted in ascending order (as defined by the list element's < operator)
        before this is called. If this is not the case then undefined behaviour, including crashes, might occur.
        All elements from the other list are transferred to this list and inserts them in the correct
        places so that the result is a properly sorted list again.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.
        
        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.     

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        
        */
    void stealAllAndMergeSorted( std::list<ELTYPE, ALLOCATOR>&& other )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge( std::move(other) );
    }



    /** Merges two sorted lists.
        Both lists *must* be sorted according to the order indicated by the \c comesBefore
        parameter.  If this is not the case then undefined behaviour, including crashes, might occur.
        
        comesBefore must be a function that takes references to two elements as parameters
        and returns true if the first one should come before the second one. See sort() for more information.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.
        
        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        
        
        */
    template<typename ComesBeforeFuncType> 
    void stealAllAndMergeSorted( List& other, ComesBeforeFuncType comesBefore )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge(
            other,
            comesBefore );
    }


    /** Merges two sorted lists.
        Both lists *must* be sorted according to the order indicated by the \c comesBefore
        parameter. If this is not the case then undefined behaviour, including crashes, might occur.
        
        comesBefore must be a function that takes references to two elements as parameters
        and returns true if the first one should come before the second one. See sort() for more information.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.
        
        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        
        
        */
    template<typename ComesBeforeFuncType> 
    void stealAllAndMergeSorted( std::list<ELTYPE, ALLOCATOR>& other, ComesBeforeFuncType comesBefore )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge(
            std::move( static_cast<std::list<ELTYPE, ALLOCATOR>&&>(other) ),
            comesBefore );
    }


    /** Merges two sorted lists.
        Both lists *must* be sorted according to the order indicated by the \c comesBefore
        parameter. If this is not the case then undefined behaviour, including crashes, might occur.
        
        comesBefore must be a function that takes references to two elements as parameters
        and returns true if the first one should come before the second one. See sort() for more information.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.
        
        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        
        
        */
    template <class ComesBeforeFuncType> 
    void stealAllAndMergeSorted( List&& other, ComesBeforeFuncType comesBefore )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge(
            std::move( static_cast<std::list<ELTYPE, ALLOCATOR>&&>(other) ),
            comesBefore );
    }


    /** Merges two sorted lists.
        Both lists *must* be sorted according to the order indicated by the \c comesBefore
        parameter.  If this is not the case then undefined behaviour, including crashes, might occur.
        
        comesBefore must be a function that takes references to two elements as parameters
        and returns true if the first one should come before the second one. See sort() for more information.

        All elements from the other list are transferred to this list and inserted in the correct
        places so that the result is a properly sorted list again.
        
        The elements from the \c other list are moved, not copied.
        The \c other list will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from the \c other list remain
        valid and refer to the merged list afterwards.        
        
        */
    template <class ComesBeforeFuncType> 
    void stealAllAndMergeSorted( std::list<ELTYPE, ALLOCATOR>&& other, ComesBeforeFuncType comesBefore )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::merge(
            std::move(other),
            comesBefore );
    }


    /** Transfers all elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition.
        
        The elements from \c otherList are moved, not copied.
        \c otherList will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAllAndInsertAt(ConstIterator insertPosition, List& otherList)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, otherList );
    }


    /** Transfers all elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition.
        
        The elements from \c otherList are moved, not copied.
        \c otherList will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAllAndInsertAt(ConstIterator insertPosition, std::list<ELTYPE, ALLOCATOR>& otherList)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, otherList );
    }

    /** Transfers all elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition.
        
        The elements from \c otherList are moved, not copied.
        \c otherList will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAllAndInsertAt(ConstIterator insertPosition, List&& otherList)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, std::move( static_cast<std::list<ELTYPE, ALLOCATOR>&>(otherList) ) );
    }


    /** Transfers all elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition.
        
        The elements from \c otherList are moved, not copied.
        \c otherList will be empty after the function returns.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAllAndInsertAt(ConstIterator insertPosition, std::list<ELTYPE, ALLOCATOR>&& otherList)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, std::move(otherList) );
    }
    



    /** Transfers a section of elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition. \c transferBeginIt must be an iterator that points to the first element in \c otherList
        to transfer. \c transferEndIt marks the end of the transfer range. It is exclusive, i.e. the element pointed to by transferEndIt is *not* transferred.
        
        The transferred elements are moved, not copied. They are removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealSectionAndInsertAt(
        ConstIterator insertPosition,
        List& otherList,
        ConstIterator transferBeginIt,
        ConstIterator transferEndIt)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, otherList, transferBeginIt, transferEndIt );
    }


    /** Transfers a section of elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition. \c transferBeginIt must be an iterator that points to the first element in \c otherList
        to transfer. \c transferEndIt marks the end of the transfer range. It is exclusive, i.e. the element pointed to by transferEndIt is *not* transferred.
        
        The transferred elements are moved, not copied. They are removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealSectionAndInsertAt(
        ConstIterator insertPosition,
        std::list<ELTYPE, ALLOCATOR>& otherList,
        typename std::list<ELTYPE, ALLOCATOR>::const_iterator transferBeginIt,
        typename std::list<ELTYPE, ALLOCATOR>::const_iterator transferEndIt)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, otherList, transferBeginIt, transferEndIt );
    }

    /** Transfers a section of elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition. \c transferBeginIt must be an iterator that points to the first element in \c otherList
        to transfer. \c transferEndIt marks the end of the transfer range. It is exclusive, i.e. the element pointed to by transferEndIt is *not* transferred.
        
        The transferred elements are moved, not copied. They are removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealSectionAndInsertAt(
        ConstIterator insertPosition,
        List&& otherList,
        ConstIterator transferBeginIt,
        ConstIterator transferEndIt)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, std::move( static_cast<std::list<ELTYPE, ALLOCATOR>&&>(otherList) ), transferBeginIt, transferEndIt );
    }

    /** Transfers a section of elements from \c otherList to this list and inserts them at the position
        indicated by \c insertPosition. \c transferBeginIt must be an iterator that points to the first element in \c otherList
        to transfer. \c transferEndIt marks the end of the transfer range. It is exclusive, i.e. the element pointed to by transferEndIt is *not* transferred.
        
        The transferred elements are moved, not copied. They are removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealSectionAndInsertAt(
        ConstIterator insertPosition,
        std::list<ELTYPE, ALLOCATOR>&& otherList,
        typename std::list<ELTYPE, ALLOCATOR>::const_iterator transferBeginIt,
        typename std::list<ELTYPE, ALLOCATOR>::const_iterator transferEndIt)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, std::move(otherList), transferBeginIt, transferEndIt );
    }




    /** Transfers one element from \c otherList to this list and inserts it at the position
        indicated by \c insertPosition. \c transferIt must be an iterator that points to the element in \c otherList
        to transfer.
        
        The transferred element is moved, not copied. It is removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAndInsertAt(
        ConstIterator insertPosition,
        List& otherList,
        ConstIterator transferIt )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, otherList, transferIt );
    }

    /** Transfers one element from \c otherList to this list and inserts it at the position
        indicated by \c insertPosition. \c transferIt must be an iterator that points to the element in \c otherList
        to transfer.
        
        The transferred element is moved, not copied. It is removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAndInsertAt(
        ConstIterator insertPosition,
        std::list<ELTYPE, ALLOCATOR>& otherList,
        typename std::list<ELTYPE, ALLOCATOR>::const_iterator transferIt )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, otherList, transferIt );
    }


    /** Transfers one element from \c otherList to this list and inserts it at the position
        indicated by \c insertPosition. \c transferIt must be an iterator that points to the element in \c otherList
        to transfer.
        
        The transferred element is moved, not copied. It is removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAndInsertAt(
        ConstIterator insertPosition,
        List&& otherList,
        ConstIterator transferIt )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, std::move( static_cast<std::list<ELTYPE, ALLOCATOR>&&>(otherList) ), transferIt );
    }

    /** Transfers one element from \c otherList to this list and inserts it at the position
        indicated by \c insertPosition. \c transferIt must be an iterator that points to the element in \c otherList
        to transfer.
        
        The transferred element is moved, not copied. It is removed from \c otherList and inserted in this list, without copying or reallocation.

        This function does *not* invalidate any iterators. Iterators from \c otherList remain
        valid and refer to the corresponding elements in the resulting combined list afterwards.        
        */
    void stealAndInsertAt(
        ConstIterator insertPosition,
        std::list<ELTYPE, ALLOCATOR>&& otherList,
        typename std::list<ELTYPE, ALLOCATOR>::const_iterator transferIt )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::splice( insertPosition, std::move(otherList), transferIt );
    }



    /** Reverses the order of the list elements.
    
        This function does *not* invalidate any iterators. No elements are copied, constructed or destructed.
        */
    void reverseOrder() noexcept
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::reverse();
    }


    /** Removes consecutive duplicates from the list.
        This is intended to be used on sorted lists (since in sorted lists all duplicates
        are next to each other).
        
        This version of the function considers to elements to be duplicates if their
        == operator returns true. There is also a variant where you can pass a custom
        duplicate check function.
        */
    void removeConsecutiveDuplicates()
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::unique();
    }


    /** Removes consecutive duplicates from the list.
        This is intended to be used on sorted lists (since in sorted lists all duplicates
        are next to each other).

        The \c isDuplicate parameter must be a function that takes two element references as its
        parameters and returns true if these elements are duplicates / equal.
        */
    template<class IsDuplicateFuncType>
    void removeConsecutiveDuplicates( IsDuplicateFuncType isDuplicate )
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::unique( isDuplicate );
    }



    /** Removes all elements that are equal to the specified one.*/
    void findAndRemove(const Element& val)
    {
        StdSequenceCollection< std::list<ELTYPE, ALLOCATOR> >::remove( val );
    }

        
    /*
        remove_if => removeCondition? removeMatching?
        remove => remove
        */

};


}


#endif
