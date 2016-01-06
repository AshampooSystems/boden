#ifndef _BDN_AppNotInstantiatedError_H_
#define _BDN_AppNotInstantiatedError_H_

#include <bdn/Error.h>

namespace bdn
{

	class AppNotInstantiatedError : public Error
	{
	public:
		AppNotInstantiatedError()
			: Error("No app object was instantiated.", "bdn", "appNotInstantiated")
		{
		}
    
	};
    
   
}



#endif


