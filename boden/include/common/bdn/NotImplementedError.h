#ifndef _BDN_NotImplementedError_H_
#define _BDN_NotImplementedError_H_

#include <bdn/Error.h>

namespace bdn
{

	class NotImplementedError : public Error
	{
	public:
		NotImplementedError(const String& funcName)
			: Error("Not implemented: "+funcName, "bdn", "notImplemented")
		{
		}

	};


}

#endif