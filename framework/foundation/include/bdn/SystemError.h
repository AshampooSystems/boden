#pragma once

#include <system_error>

namespace bdn
{

    /** Indicates an error reported by the operating system or another component
       that returns operating system error codes.

        If you want to throw a SystemError then you should take a look at
       throwSysError() and BDN_throwLastSysError().

        Note that this is implemented as a typedef to std::system_error.
        So SystemError is actually the SAME type as std::system_error.
        That means that the following will catch both bdn::SystemError AND
       std::system_error exceptions.

        \code

        try
        {
        ...
        }
        catch(SystemError& e)	// will catch std::system_error as well
        {
        }

        \endcode
    */
    typedef std::system_error SystemError;
}
