#pragma once

namespace bdn
{

    /** Sort comparison function that orders the elements in ascending order
        (small elements first). This is intended to be used with collection
       sort() calls of collection classes. See, for example, Array::sort() and
       List::sort().
        */
    template <typename ElementType> inline bool ascending(const ElementType &a, const ElementType &b) { return a < b; }

    /** Sort comparison function that orders the elements in descending order
        (small elements first). This is intended to be used with collection
       sort() calls of collection classes. See, for example, Array::sort() and
       List::sort().
        */
    template <typename ElementType> inline bool descending(const ElementType &a, const ElementType &b) { return b < a; }
}
