#ifndef BDN_sequenceUtil_H_
#define BDN_sequenceUtil_H_

#include <bdn/SequenceFilter.h>

namespace bdn
{

    /** Provides a filtered version of the specified base \ref sequence.md
       "sequence", according to the specified filtering function. The resulting
       sequence is "filtered" in the sense that some elements of the underlying
       sequence may be skipped. The specified filtering function controls Which
       elements are skipped.

        A "sequence" is simply an object that provides begin() and end()
       functions, which return \ref iterator.md "iterator" objects to the
       elements of the sequence (\ref sequence.md "more information").

        The returned object is itself a sequence, i.e. it also provides begin()
       and end() functions, which return iterators to the filtered sequence of
       elements.

        filterSequence is a template function with the following template
       parameters:

        - BaseSequenceType: the type of the base sequence. For example, if an
       array of integers is filtered then this would be bdn::Array<int>.
        - FilterFuncType: the type of the filter function (see below)

        The filter function gets two parameters: a reference to the base
       sequence object and an iterator from the base sequence. It must then
       examine the iterator and if it points to an element that should be
       skipped it should advance it to the next element that should not be
       skipped (or set it to end() if all elements from the iterator position to
       the end should be skipped).

        Note that it is recommended to let the compiler deduce the types of the
       base sequence and the filter function automatically, based on the
       provided parameters.

        Example:

        \code

        // the base sequence consists of the numbers 1 to 5 in a random order
        Array<int> baseSequence { 2, 5, 3, 4, 1 };

        // now we create a filtered sequence that filters out all elements that
       are bigger than 3

        auto filteredSequence = filterSequence( baseSequence,
                                                []( Array<int>& baseSequence,
       Array<int>::Iterator it )
                                                {
                                                    while(
       it!=baseSequence.end() && *it > 3 )
                                                        ++it;
                                                    return it;
                                                } );

        // the filtered sequence will be 2, 3, 1 (the order from the original
       sequence).
        // The 5 and 4 elements are skipped.

        // we can now iterator over the filtered sequence, either by calling the
        // filteredSequence.begin() and filteredSequence.end() methods.
        // Even simpler, we can use the C++ range-based for-loop:

        for( auto& element : filteredSequence )
        {
            ...
        }

        \endcode

        */
    template <class BaseSequence, class FilterFuncType>
    SequenceFilter<BaseSequence, FilterFuncType>
    filterSequence(BaseSequence &baseSequence, FilterFuncType filterFunc)
    {
        return SequenceFilter<BaseSequence, FilterFuncType>(baseSequence,
                                                            filterFunc);
    }
}

#endif
