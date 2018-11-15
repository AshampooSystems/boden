#ifndef BDN_SequenceFilter_H_
#define BDN_SequenceFilter_H_

#include <bdn/config.h>

#include <iterator>
#include <type_traits>

namespace bdn
{

#if BDN_SEQUENCE_FILTER_COMPILER_BUG

    /** Helper used to work around a compiler bug in Visual Studio 2015 and
     * 2017. Do not use.*/
    template <typename BaseSequence> struct MSCSequenceFilterIteratorTypedefHelper_
    {
        using type = decltype(std::begin(*((BaseSequence *)nullptr)));
    };

#endif

    /** Implements a filter on top of a provided \ref sequence.md "sequence" of
       elements, using a provided filtering function.

        It is usually more convenient to use the filterSequence() utility
       function instead of manually creating the SequenceFilter object. When
       using filterSequence the compiler can automatically deduce the necessary
       template parameters from the function arguments.

        The SequenceFilter object works on top of the provided base \ref
       sequence.md "sequence", which can be of any type. The SequenceFilter then
       uses the provided filtering function to filter out (skip) some of the
       elements of the underlying base sequence. The result is a new sequence -
       i.e. SequenceFilter is itself a \ref sequence.md "sequence" object.

        SequenceFilter is a template with the following template parameters:

        - BaseSequenceType: the type of the base sequence. For example, if an
       integer Array is filtered then this would be bdn::Array<int> .
        - FilterFuncType: the type of the filter function (see below)

        The filter function must take one parameter: an iterator from the base
       sequence. It must then skip over any elements that should NOT be in the
       filtered sequence and return the resulting iterator to the next element
        that is included in the filtered sequence. If no such element exists
       then the end iterator of the base sequence should be returned.

        The filter function is NEVER called with an iterator that points to the
       end of the base sequence. The iterator passed as a parameter to the
       filter function ALWAYS initially points to a valid element of the base
       sequence. I.e. it is NEVER the end() iterator of the base seqeunce.

        Example:

        \code

        // the base sequence consists of the numbers 1 to 5
        Array<int> baseSequence {1, 2, 3, 4, 5};

        SequenceFilter< Array<int>, Array<int>::Iterator,

        */
    template <class BaseSequenceType, class FilterFuncType> class SequenceFilter
    {
      public:
        using BaseSequence = BaseSequenceType;

#if BDN_SEQUENCE_FILTER_COMPILER_BUG
        // Visual Studio 2017 and below
        // this is a workaround for a bug in VS2015. The commented out line is
        // the simple, normal type alias we want: BaseIterator should be the
        // result of the begin function for the given sequence. BUT
        // unfortunately in VS2015 there is a compiler bug. While the type alias
        // works in general, it does not produce a type that can be used to
        // define a conversion operator (VS produces this error message:
        // 'operator decltype' is not a recognized operator or type). But if we
        // let the compiler jump through a few hoops then this works. We define
        // the BaseIterator as the std::result_of of a function that returns the
        // iterator. This works on VS2015.

        using BaseIterator = typename MSCSequenceFilterIteratorTypedefHelper_<BaseSequence>::type;

#else
        using BaseIterator = decltype(std::begin(*((BaseSequence *)nullptr)));

#endif

        using Element = typename std::iterator_traits<BaseIterator>::value_type;

        class Iterator
            : public std::iterator<std::forward_iterator_tag, typename std::iterator_traits<BaseIterator>::value_type,
                                   typename std::iterator_traits<BaseIterator>::difference_type,
                                   typename std::iterator_traits<BaseIterator>::pointer,
                                   typename std::iterator_traits<BaseIterator>::reference>
        {
          public:
            Iterator() : _pFilter(nullptr) {}

            Iterator(const Iterator &o) : _pFilter(o._pFilter), _baseIt(o._baseIt) {}

            Iterator(Iterator &&o) : _pFilter(o._pFilter), _baseIt(std::move(o._baseIt)) {}

            Iterator(const SequenceFilter &filter, const BaseIterator &baseIt) : _pFilter(&filter), _baseIt(baseIt) {}

            Iterator(const SequenceFilter &filter, BaseIterator &&baseIt)
                : _pFilter(&filter), _baseIt(std::move(baseIt))
            {}

            operator BaseIterator() { return _baseIt; }

            Iterator &operator=(Iterator &&other)
            {
                _pFilter = other._pFilter;
                _baseIt = std::move(other._baseIt);
                return *this;
            }

            Iterator &operator=(const Iterator &other)
            {
                _pFilter = other._pFilter;
                _baseIt = other._baseIt;
                return *this;
            }

            Iterator &advanceAfterRemoval(const BaseIterator &baseIt)
            {
                if (_pFilter == nullptr)
                    throw std::logic_error("SequenceFilter::Iterator::advanceAfterRemoval() "
                                           "called, but no filter object is associated with the "
                                           "iterator.");

                _baseIt = baseIt;
                _pFilter->_skipExcluded(_baseIt);

                return *this;
            }

            Iterator &advanceAfterRemoval(BaseIterator &&baseIt)
            {
                if (_pFilter == nullptr)
                    throw std::logic_error("SequenceFilter::Iterator::advanceAfterRemoval() "
                                           "called, but no filter object is associated with the "
                                           "iterator.");

                _baseIt = std::move(baseIt);
                _pFilter->_skipExcluded(_baseIt);

                return *this;
            }

            BaseIterator getBaseIterator() const { return _baseIt; }

            const typename std::iterator_traits<BaseIterator>::value_type &operator*() const { return *_baseIt; }

            typename std::iterator_traits<BaseIterator>::value_type *operator->() const { return &*_baseIt; }

            bool operator==(const Iterator &other) const
            {
                return (_pFilter == other._pFilter && _baseIt == other._baseIt);
            }

            bool operator!=(const Iterator &other) const
            {
                return (_pFilter != other._pFilter || _baseIt != other._baseIt);
            }

            Iterator &operator++()
            {
                ++_baseIt;

                _pFilter->_skipExcluded(_baseIt);

                return *this;
            }

            Iterator operator++(int)
            {
                Iterator oldVal = *this;
                operator++();

                return oldVal;
            }

          private:
            const SequenceFilter *_pFilter;
            BaseIterator _baseIt;
        };
        friend class Iterator;

        // aliases for compatibility with standard library
        using iterator = Iterator;
        using value_type = Element;

        SequenceFilter(BaseSequence &baseSequence, FilterFuncType filterFunc)
            : _baseSequence(baseSequence), _filterFunc(filterFunc)
        {}

        Iterator begin() const
        {
            BaseIterator initial = _baseSequence.begin();
            _skipExcluded(initial);

            return Iterator{*this, initial};
        }

        Iterator end() const { return Iterator{*this, _baseSequence.end()}; }

      private:
        void _skipExcluded(BaseIterator &baseIt) const
        {
            if (baseIt != _baseSequence.end())
                _filterFunc(_baseSequence, baseIt);
        }

        BaseSequence &_baseSequence;
        mutable FilterFuncType _filterFunc;
    };
}

#endif
