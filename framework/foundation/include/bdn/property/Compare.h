#pragma once

#include <functional>

namespace bdn
{
    template <class T> struct Compare
    {
        static const bool is_faked = false;
        static bool not_equal(const T &left, const T &right) { return left != right; }
    };

    template <class _fp> struct Compare<std::function<_fp>>
    {
        static const bool is_faked = true;
        static bool not_equal(const std::function<_fp> &left, const std::function<_fp> &right) { return true; }
    };
}
