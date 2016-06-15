#ifndef BDN_ViewTypeNotSupportedError_H_
#define BDN_ViewTypeNotSupportedError_H_

#include <stdexcept>

namespace bdn
{

/** A view type was used that is not supported.*/
class ViewTypeNotSupportedError : public std::runtime_error
{
public:
	ViewTypeNotSupportedError(const String& viewTypeName)
		: std::runtime_error("The view type '"+viewTypeName+"' is not supported.")
	{
	}

};


}

#endif
