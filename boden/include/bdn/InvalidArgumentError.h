#ifndef BDN_InvalidArgumentError_H_
#define BDN_InvalidArgumentError_H_

#include <stdexcept>

namespace bdn
{

/** An invalid argument was passed.

	This usually indicates a programming error.

	Note that this is actually implemented as a typedef to std::invalid_argument.
	So InvalidArgError is actually the SAME type as std::invalid_argument.
	That means that the following will catch both bdn::InvalidArgumentError AND std::invalid_argument
	exceptions.

	\code

	try
	{
	...
	}
	catch(InvalidArgumentError& e)	// will catch std::invalid_argument as well
	{
	}

	\endcode
*/
#ifdef BDN_GENERATING_DOCS
class InvalidArgumentError : public std::invalid_argument, public ProgrammingError
{
public:
	explicit InvalidArgumentError( const std::string& message );
	explicit InvalidArgumentError( const char* message );
};

#else

typedef std::invalid_argument InvalidArgumentError;

#endif


}


#endif

