#ifndef BDN_typeUtil_H_
#define BDN_typeUtil_H_

namespace bdn
{

    /** Internal helper for typeSupportsShiftLeftWith().

        Determines whether the statement x << y is valid for two objects.
        The template parameters TYPE and ARG_TYPE indicate the types
        of the left and right operand of the statement (x and y in this
       example).

        If the << statement is valid then TypeSupportsShiftLeftWith_< XType,
       YType >::value is true, otherwise it is false.
     */
    template <class LEFT_TYPE, typename ARG_TYPE> struct TypeSupportsShiftLeftWith_
    {
      private:
        // note: we MUST NOT use std::declval<TYPE>() here.
        // That returns an rvalue reference.  We want this template to work with
        // streams and there is a generic conversion operator for rvalue streams
        // that takes any parameter and simply calls the lvalue operator. Since
        // its return type does not depend on the template parameter, the
        // compiler might decide that it can determine the return type without
        // looking at the implementation of the generic template rvalue
        // operator. So it might decide that std::declval<TYPE>() << XYZ is
        // valid for all XYZ - which is not what we want. So we use
        // *((TYPE*)nullptr) instead, which is an lvalue reference.

        template <typename TEST_ARG_TYPE>
        static uint8_t _test(int dummy,
                             decltype(&((*(LEFT_TYPE *)nullptr) << std::declval<TEST_ARG_TYPE>())) pDummy = nullptr);

        template <typename TEST_ARG_TYPE> static uint16_t _test(...);

        // this is a workaround for a compiler bug in the Visual Studio 2015
        // compiler for Windows Universal. If we do not have this static_assert
        // here then the test below will always fail, for all value types.
        // Probably has something to do with the fact that the static_assert
        // instantiates the template, which causes the bug not to be triggered.
        static_assert(sizeof(_test<ARG_TYPE>(0)) != 0, "This should never trigger");

      public:
        enum
        {
            value = sizeof(_test<ARG_TYPE>(0)) == sizeof(uint8_t) ? 1 : 0
        };
    };

    /** Returns true if the statement "x << y" is valid for two objects x and y
       of the types TYPE and ARG_TYPE (specified via template parameters).

        typeSupportsShiftLeftWith is a constexpr function, so it can also be
       used in compile time statements.

        Example:

        \code

        // The following returns true, since std::ostream overloads the
        // the << operator for arguments of type int.
        typeSupportsShiftLeftWith< std::ostream, int>();

        // the next statement also returns true, since there is a builtin shift
       operator
        // for integer types (doing a bitwise shift).
        typeSupportsShiftLeftWith< int, int >();

        // the next statement returns false, since the std::vector class does
       not
        // provide a << operator for any argument.
        typeSupportsShiftLeftWith< std::vector, std::string >();

        \endcode
        */
    template <class TYPE, typename ARG_TYPE> constexpr bool typeSupportsShiftLeftWith()
    {
        return TypeSupportsShiftLeftWith_<TYPE, ARG_TYPE>::value;
    }

    /** Internal helper for typeHasCustomArrowOperator().*/
    template <typename TYPE> struct TypeHasCustomArrowOperator_
    {
      private:
        // note that we do not use std::declval here, since that returns an
        // rvalue reference. Since we are only allowed to call const functions
        // on rvalue references with some compilers, we would not be able to
        // detect non-const operator-> overloads

        template <typename TEST_TYPE>
        static uint8_t _test(int dummy, decltype(((TEST_TYPE *)nullptr)->operator->()) *pDummy = nullptr);

        template <typename TEST_TYPE> static uint16_t _test(...);

        static_assert(sizeof(_test<TYPE>(0)) != 0, "This should never trigger");

      public:
        enum
        {
            value = sizeof(_test<TYPE>(0)) == sizeof(uint8_t) ? 1 : 0
        };
    };

    /** Returns true if the type indicated by the template argument
        provides a custom arrow operator (operator->).

        Note that this returns false for primitive pointer types, since
        those have a buitin arrow operator, not a custom one.

        typeHasCustomArrowOperator is a constexpr function, so it can also be
       used in compile time statements.
     */
    template <class TYPE> constexpr bool typeHasCustomArrowOperator()
    {
        return TypeHasCustomArrowOperator_<TYPE>::value;
    }
}

#endif
