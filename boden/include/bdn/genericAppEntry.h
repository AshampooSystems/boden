#ifndef BDN_genericAppEntry_H_
#define BDN_genericAppEntry_H_

#include <bdn/IAppRunner.h>
#include <bdn/AppControllerBase.h>

namespace bdn
{



/** A generic implementation for a commandline app entry function.
	You do not normally have to call this directly. use the BDN_APP_INIT macro instead.*/
int genericCommandLineAppEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, int argc, char* argv[]);

}


#endif
