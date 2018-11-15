#ifndef BDN_Map_H_
#define BDN_Map_H_

#include <map>

#include <bdn/StdMapCollection.h>

namespace bdn
{

    /** A container that stores key-value pairs. The key is used to access the
       value very efficiently.

        Key ordering
        ------------

        The Map class requires that there is a well defined ordering among the
       keys. Any ordering is fine, but Map must be able to determine if key A
       comes before key B in that ordering. This information is used to optimize
       the data structures and enable efficient access.

        By default the Map class uses the < operator on the keys to determine
       their ordering. This can be changed with the third template parameter
       (for example, if the keys are objects that do not support the <
       operator). If specified, the third template parameter must be the type of
       a compare function object. An instance of this compare function object
       type can be passed to the constructor of Map. If no compare object is
       passed to the constructor then one is automatically created.

        Keys A and B are considered to be equal if neither one is smaller than
       the other. I.e. Neither A < B nor B < A is true with "<" being the
       ordering of the elements in the set.

        Iteration order
        ---------------

        Iterators of the Map class (see begin()) return the elements in "small
       first" order (according to the key ordering that the Map uses - see
       above).


        Base classes and std::map compatibility
        ---------------------------------------

        The Map class is derived from std::map and is 100 % compatible with it.
        It can be used as a drop-in replacement.

        Map provides renamed aliases for some std::map methods and types to
       ensure that they match the naming conventions used otherwise in the
       framework. The new names are also intended to make using the class more
       intuitive for newcomers that may not be familiar with the C++ standard
       library.

        Note that Map is also derived from bdn::Base, so it can be used with
       smart pointers (see bdn::P).

        Allocator
        ---------

        Like the C++ standard library collections, one can specify an
       "allocator" type as an optional third template argument. This is used
       when special custom memory management is needed. If you do not need that
       then you should ignore the ALLOCATOR template argument and leave it at
       the default.

    */
    template <typename KEYTYPE, typename VALTYPE, typename COMPAREFUNCTYPE = std::less<KEYTYPE>,
              class ALLOCATOR = std::allocator<std::pair<const KEYTYPE, VALTYPE>>>
    class Map : public StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>
    {
      public:
        using typename StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>::Element;
        using typename StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>::Iterator;
        using typename StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>::ConstIterator;

        /** The class of iterator objects for iteration over the collection
         * elements in reverse order (with read/write access to the elements).*/
        using ReverseIterator =
            typename StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>::reverse_iterator;

        /** The class of iterator objects for iteration over the collection
         * elements in reverse order (with read-only access to the elements).*/
        using ConstReverseIterator =
            typename StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>::const_reverse_iterator;

        Map() : Map(COMPAREFUNCTYPE()) {}

        explicit Map(const COMPAREFUNCTYPE &compareFunc, const ALLOCATOR &alloc = ALLOCATOR())
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(compareFunc, alloc)
        {}

        explicit Map(const ALLOCATOR &alloc)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(alloc)
        {}

        template <class InputIt>
        Map(InputIt beginIt, InputIt endIt, const COMPAREFUNCTYPE &compareFunc = COMPAREFUNCTYPE(),
            const ALLOCATOR &alloc = ALLOCATOR())
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(beginIt, endIt, compareFunc,
                                                                                       alloc)
        {}

        template <class InputIt>
        Map(InputIt beginIt, InputIt endIt, const ALLOCATOR &alloc)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(beginIt, endIt,
                                                                                       COMPAREFUNCTYPE(), alloc)
        {}

        Map(const Map &other) : Map(static_cast<const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &>(other))
        {}

        Map(const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &other)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(other)
        {}

        Map(const Map &other, const ALLOCATOR &alloc)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(other, alloc)
        {}

        Map(const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &other, const ALLOCATOR &alloc)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(other, alloc)
        {}

        Map(Map &&other)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(
                  std::move(static_cast<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &&>(other)))
        {}

        Map(std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &&other)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(std::move(other))
        {}

        Map(Map &&other, const ALLOCATOR &alloc)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(
                  std::move(static_cast<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &&>(other)), alloc)
        {}

        Map(std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &&other, const ALLOCATOR &alloc)
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(std::move(other), alloc)
        {}

        Map(std::initializer_list<Element> initList, const COMPAREFUNCTYPE &compFunc = COMPAREFUNCTYPE(),
            const ALLOCATOR &alloc = ALLOCATOR())
            : StdMapCollection<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>>(initList, compFunc, alloc)
        {}

        Map(std::initializer_list<Element> initList, const ALLOCATOR &alloc) : Map(initList, COMPAREFUNCTYPE(), alloc)
        {}

        /** Replaces the current contents of the map with copies of the elements
           from the specified other Map.

            Returns a reference to this Map object.
            */
        Map &operator=(const Map &other)
        {
            std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=(other);
            return *this;
        }

        /** Replaces the current contents of the map with copies of the elements
           from the specified std::map object.

            Returns a reference to this Map object.
            */
        Map &operator=(const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &other)
        {
            std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=(other);
            return *this;
        }

        /** Replaces the current contents of the set with copies of the elements
           from the specified initializer list. This is called by the compiler
           if a  "= {...} " statement is used.

           Note that each element is a Key, Value pair (a std::pair<const Key,
           Value> object).

           For example:

           \code
           // a map object that maps integers to their textual representation
           (just an example) Map<int, String> myMap;

           // we now add three elements to the map. Each is a pair of an integer
           and a string.

           myMap = {   {1, "one"}
                       {17, "seventeen"}
                       {42, "fortytwo"}
                   };

           \endcode

           Returns a reference to this Map object.
           */
        Map &operator=(std::initializer_list<Element> initList)
        {
            std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=(initList);
            return *this;
        }

        /** Moves the data from the specified other Map object to this set,
           replacing any current contents in the process. The other Map object
           is invalidated by this operation.
            */
        Map &operator=(Map &&other)
        {
            std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=(std::move(other));
            return *this;
        }

        /** Moves the data from the specified other Map object to this set,
           replacing any current contents in the process. The other Map object
           is invalidated by this operation.
            */
        Map &operator=(std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> &&other)
        {
            std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=(std::move(other));
            return *this;
        }

        /** Returns an iterator that is intended for iteration over the
           collection elements in reversed order.

            The returned iterator points to the last element of the collection.

            The directions of its ++ and -- operator are inverted: the ++
           operator moves the iterator to the previous element, -- moves it to
           the next element.

            Reverse iterators should only be compared to other reverse
           iterators. reverseEnd() returns an iterator that marks the end point
           of the reverse iteration (analogous to end() for forward iteration).

            The following code example iterates over the collection elements in
           reversed order:

            \code

            Array<int> ar = {1, 5, 7};

            auto it = ar.reverseBegin();    // iterator initially points to the
           last element while( it != ar.reverseEnd() )
            {
                // do something with the element
                ...

                // go to the previous element of the collection
                ++it;
            }

            \endcode

            */
        ReverseIterator reverseBegin() noexcept { return this->rbegin(); }

        /** Const version of reverseBegin() - returns an iterator for read-only
         * access to the elements in reversed order.
         */
        ConstReverseIterator reverseBegin() const noexcept { return this->rbegin(); }

        /** Const version of reverseBegin() - returns an iterator for read-only
         * access to the elements in reversed order.
         */
        ConstReverseIterator constReverseBegin() const noexcept { return this->crbegin(); }

        /** Marks the end point of a reverse iteration.

            This is used together with reverseBegin() to iterate over the
           collection elements in reverse order. It behaves analogous to end():
           it points to a collection position just before the first element.

            Like end(), the iterator returned by reverseEnd() does not point to
           a valid element - it simply marks the end position for the reverse
           iteration. See the documentation of reverseBegin() for more
           information on how to use it.
        */
        ReverseIterator reverseEnd() noexcept { return this->rend(); }

        /** Const version of reverseEnd() - returns an iterator for read-only
         * access to the elements in reversed order.
         */
        ConstReverseIterator reverseEnd() const noexcept { return this->rend(); }

        /** Const version of reverseEnd() - returns an iterator for read-only
         * access to the elements in reversed order.
         */
        ConstReverseIterator constReverseEnd() const noexcept { return this->crend(); }
    };
}

#endif
