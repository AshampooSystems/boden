#ifndef BDN_WIN32_CommandLineAppRunner_H_
#define BDN_WIN32_CommandLineAppRunner_H_

#include <bdn/GenericCommandLineAppRunner.h>
#include <bdn/win32/util.h>

namespace bdn
{
namespace win32
{
    
    
class CommandLineAppRunner : public GenericCommandLineAppRunner
{
public:
	CommandLineAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[])
		: GenericCommandLineAppRunner( appControllerCreator, makeAppLaunchInfo() )
	{
	}
	

protected:
	void platformSpecificInit() override;
};
  		

    
}    
}

#endif

