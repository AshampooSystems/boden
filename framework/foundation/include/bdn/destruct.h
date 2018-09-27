#ifndef BDN_destruct_H_
#define BDN_destruct_H_

namespace bdn
{

    /** Used internally. Do not call.*/
    void _handleDestructorException(const char *className,
                                    const std::exception &e) noexcept;

    /** Used internally. Do not call.*/
    void _handleDestructorException(const char *className) noexcept;
}

/** \def BDN_DESTRUCT_BEGIN

    Used together with BDN_DESTRUCT_END to handle exceptions in destructors.

    Exceptions in destructors can be problematic because destructors are
   commonly called during exception handling (stack frame unwinding). And if a
   destructor throws an exception during stack frame unwinding then the program
   will be terminated.

    The BDN_DESTRUCT_BEGIN and BDN_DESTRUCT_END macros are designed to prevent
   such crashes. They should be used at the start and end of all destructors.

    They will catch all exceptions that happen in the destructor, log them and
   prevent them from being propagated.

    Example:

    \code

    class MyClass
    {
    public:
        ~MyClass()
        {
            BDN_DESTRUCT_BEGIN;

            ... destructor code

            BDN_DESTRUCT_END( MyClass );
        }
    };

    \endcode

*/
#define BDN_DESTRUCT_BEGIN try {

/** \def BDN_DESTRUCT_END(className)

    Marks the end of a destructor.

    className must be the name of the class being destructed. It is included in
   log messages.

    See #BDN_DESTRUCT_BEGIN for more information
*/
#define BDN_DESTRUCT_END(className)                                            \
    }                                                                          \
    catch (std::exception & e)                                                 \
    {                                                                          \
        bdn::_handleDestructorException(#className, e);                        \
    }                                                                          \
    catch (...) { bdn::_handleDestructorException(#className); }

#endif
