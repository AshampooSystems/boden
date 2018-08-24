#ifndef BDN_OutOfRangeError_H_
#define BDN_OutOfRangeError_H_

#include <stdexcept>

namespace bdn
{

/** An attempt was made to access an element outside the valid range.

	For example, an invalid index was passed to a string or array, or something like that.

	This usually indicates a programming error.

	Note that this is actually implemented as a typedef to std::out_of_range.
	So OutOfRangeError is actually the SAME type as std::out_of_range.
	That means that the following will catch both bdn::OutOfRangeError AND std::out_of_range
	exceptions.

	\code

	try
	{
	...
	}
	catch(OutOfRangeError& e)	// will catch std::out_of_range as well
	{
	}

	\endcode
*/
#ifdef BDN_GENERATING_DOCS
class OutOfRangeError : public std::out_of_range, public ProgrammingError
{
public:
	explicit OutOfRangeError( const std::string& message );
	explicit OutOfRangeError( const char* message );
};

#else

typedef std::out_of_range OutOfRangeError;

#endif


}


#endif

