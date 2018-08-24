#ifndef BDN_Array_H_
#define BDN_Array_H_

#include <vector>

#include <bdn/StdPositionalCollection.h>

namespace bdn
{

/** Implements an array of element the type indicated by the first template
    parameter. For example Array<int> is an array of integers.

    The Array class is derived from std::vector and is 100 % compatible with it.
    It can be used as a drop-in replacement.

    Array provides renamed aliases for some std::vector methods and types to ensure that
    they match the naming conventions used otherwise in the framework. The new
    names are also intended to make using the class more intuitive for newcomers that may not be familiar with
    the C++ standard library.

    Note that Array is also derived from bdn::Base, so it can be used with smart pointers (see bdn::P).
    
    Additionally, some utility and convenience functions have been added to Array that std::vector
    does not have to make commonly used functionality more convenient (like sorting, etc).

    Like the C++ standard library collections, one can specify an "allocator" type as an optional
    second template argument. This is used when special custom memory management is needed.
    If you do not need that then you should ignore the ALLOCATOR template argument and leave it at the default. 

*/
template<typename ELTYPE, class ALLOCATOR = std::allocator<ELTYPE> >
class Array : public StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >
{
public:
    
    using typename StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::Element;
    using typename StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::Size;
    using typename StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::Iterator;
    using typename StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::ConstIterator;
    using typename StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::ReverseIterator;
    using typename StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::ConstReverseIterator;
    
    /** Creates an empty array.*/
    Array() noexcept( noexcept(ALLOCATOR()) )
     : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >( ALLOCATOR() )
    {
    }

    /** Constructs an array that uses a specific allocator object.*/
    explicit Array( const ALLOCATOR& alloc ) noexcept
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >(alloc)
    {
    }


    /** Initializes the array with \c count copies of \c el. 
    
        Optionally, one can also pass an allocator object for custom memory management.
    */
    Array( Size count, const Element& el, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >(count, el, alloc)
    {
    }


    /** Initializes the array with \c count default-constructed elements. 
    
        Optionally, one can also pass an allocator object for custom memory management.
    */
    explicit Array( Size count )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >(count)
    {
    }
    

    /** Initializes the array with copies of the elements from the iterator range [beginIt ... endIt) 

        Optionally, one can also pass an allocator object for custom memory management.
    */
    template< class InputIt >
    Array( InputIt beginIt, InputIt endIt, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >(beginIt, endIt, alloc)
    {
    }

    /** Initializes the Array with copies of the elements from the specified other array.*/
    Array( const Array& other )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >( static_cast<const std::vector<ELTYPE, ALLOCATOR>&>(other) )
    {
    }


    /** Initializes the Array with copies of the elements from the specified std::vector object.*/
    Array( const std::vector<ELTYPE, ALLOCATOR>& other )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >(other)
    {
    }


    /** Initializes the Array with copies of the elements from the specified std::vector or Array object.
        
        The specified allocator object is used to initialize the array's internal allocator.
        */
    Array( const std::vector<ELTYPE, ALLOCATOR>& other, const ALLOCATOR& alloc )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >(other, alloc)
    {
    }

    /** Moves the data from the specified other array to this array. The other array is invalidated by this.*/
    Array( Array&& other ) noexcept
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >( std::move( static_cast<std::vector<ELTYPE, ALLOCATOR>&&>(other) ) )
    {
    }


    /** Moves the data from the specified other vector to this array. The other array is invalidated by this.*/
    Array( std::vector<ELTYPE, ALLOCATOR>&& other ) noexcept
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >( std::move(other) )
    {
    }


    /** Moves the data from the specified other array to this array. The other array is invalidated by this.
    
        The specified allocator object is used to initialize the array's internal allocator.
        */
    Array( Array&& other, const ALLOCATOR& alloc )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >( std::forward(other), alloc )
    {
    }


    /** Moves the data from the specified other vector to this array. The other array is invalidated by this.
    
        The specified allocator object is used to initialize the array's internal allocator.*/
    Array( std::vector<ELTYPE, ALLOCATOR>&& other, const ALLOCATOR& alloc )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >( std::forward(other), alloc )
    {
    }


    /** Initializes the array with the specified initializer list. This is called by the compiler
        when {...} initialization is used (see example below).

        Optionally, one can also pass an allocator object for custom memory management.

        Example:

        \code

        // initialize the array with these elements: 1, 13, 42
        Array<int> ar { 1, 13, 42 };

        \endcode
    */
    Array( std::initializer_list<Element> initList, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >( initList, alloc )
    {
    }


    /** Replaces the current contents of the array with copies of the elements from the specified other
        Array.
        
        Returns a reference to this Array object.
        */
    Array& operator=( const Array& other )
    {
        std::vector<ELTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


    /** Replaces the current contents of the array with copies of the elements from the specified
        vector.
        
        Returns a reference to this Array object.
        */
    Array& operator=( const std::vector<ELTYPE, ALLOCATOR>& other )
    {
        std::vector<ELTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


     /** Replaces the current contents of the array with copies of the elements from the specified
        initializer list. This is called by the compiler if a  "= {...} " statement is used. For example:

        \code
        Array<int> ar;

        ar = {1, 17, 42};
        \endcode

        Returns a reference to this Array object.
        */
    Array& operator=( std::initializer_list<Element> initList )
    {
        std::vector<ELTYPE, ALLOCATOR>::operator=(initList);
        return *this;
    }  




    /** Moves the data from the specified other Array object to this array, replacing any current contents in the process.
        The other Array object is invalidated by this operation.
        */
    Array& operator=( Array&& other )
    {
        std::vector<ELTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }
    

    /** Moves the data from the specified other Array object to this array, replacing any current contents in the process.
        The other Array object is invalidated by this operation.
        */
    Array& operator=( std::vector<ELTYPE, ALLOCATOR>&& other )
    {
        std::vector<ELTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }


    /** Returns a reference to the element at the specified zero-based index.

        The reference can be used to modify the element in-place.
    */
    Element& atIndex( Size index )
    {
        return this->at(index);
    }


    /** Const version of atIndex() -- returns a const reference to the element at the specified zero-based index.
    */
    const Element& atIndex( Size index ) const
    {
        return this->at(index);
    }


    /** [] operator that returns a reference to the element at the specified zero-based index.

        The reference can be used to modify the element in-place.

        Example:

        \code

        Array<int> ar = {7, 42, 117};

        int el = ar[1]; // el will be 42

        ar[1] = 23;     // the 42 element will be overwritten with the number 23

        \endcode
    */
    Element& operator[]( Size index )
    {
        return this->at(index);
    }


    /** Const version of operator[] -- returns a const reference to the element at the specified zero-based index.
    */
    const Element& operator[]( Size index ) const
    {
        return this->at(index);
    }

    
    /** Returns a pointer to the underlying raw array data.*/
    Element* getData() noexcept
    {
        return StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::data();
    }
    
    /** Const version of getData() - returns a const pointer to the underlying raw array data.*/
    const Element* getData() const noexcept
    {
        return StdPositionalCollection< std::vector<ELTYPE, ALLOCATOR> >::data();
    }




    /** Prepares the array for a bigger insert operation. This is purely for optimization
        purposes and can be used to prevent intermediate re-allocations when a lot of elements are added.

        The function will ensure that the internal buffer can hold at least the specified
        total number of elements. This number of elements includes any current elements that the array may have.

        Note that the number of elements of the array is not modified by this function - it only optimizes 
        internal memory management.
    */
    void prepareForSize(Size size)
    {
        this->reserve(size);
    }


    /** Returns the zero based index that corresponds to the specified iterator.*/
    Size iteratorToIndex( ConstIterator it ) const
    {
        return it - this->begin();
    }


    /** Returns an iterator to the element at the specified zero based index.
    
        If the index equals the size of the array then the end() iterator is returned.
    */
    Iterator indexToIterator( Size index )
    {
        return this->begin()+index;
    }

    
    /** Const version of indexToIterator() - returns an const iterator to the element
        at the specified zero based index.*/
    ConstIterator indexToIterator( Size index ) const
    {
        return this->begin()+index;
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
            Array<String> myArray = ...;

            // sort in ascending order
            myArray.sort(); 

            // sort in descending order. bdn::descending() is a predefined
            // comparison function provided by the boden framework.
            myArray.sort( descending<String> );

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
    void sort()
    {
        std::sort( this->begin(), this->end() );
    }


    /** Sorts the elements in a custom order. comesBefore must be a function that
        takes references to two elements as its parameters and returns true if the first one
        should come before the second one. There are also two predefined comparison functions
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
            myArray.sort( descending<String> );

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
        std::sort( this->begin(), this->end(), comesBefore );
    }


    /** Like sort() except that the sorting algorithm is guaranteed to be stable. See sort() documentation for
        more information about what this means.
    */
    void stableSort()
    {
        std::stable_sort( this->begin(), this->end() );
    }


    /** Like sort() except that the sorting algorithm is guaranteed to be stable. See sort() documentation for
        more information about what this means.
    */
    template<class ComesBeforeFuncType>
    void stableSort( ComesBeforeFuncType comesBeforeFunc )
    {
        std::stable_sort( this->begin(), this->end(), comesBeforeFunc );
    }



	

    /** Removes all elements that are equal to the specified one.*/
    void findAndRemove(const Element& val)
    {
		auto it = this->begin();
		while( it != this->end() )
		{
			if( *it == val )
				it = this->removeAt(it);
			else
				++it;
		}
    }


    /** Removes all elements for which the specified function matchFunc returns true.
    
        matchFunc must be a function that takes a collection iterator as its parameter
        and returns true if the element should be removed.
    */
    template<typename MATCH_FUNC_TYPE>
    void findCustomAndRemove( MATCH_FUNC_TYPE&& matchFunc )
    {
		auto it = this->begin();
		while( it != this->end() )
		{
			if( matchFunc(it) )
				it = this->removeAt(it);
			else
				++it;
		}
    }


};


}


#endif
