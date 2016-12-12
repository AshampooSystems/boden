#ifndef BDN_WIN32_CommandLineAppRunner_H_
#define BDN_WIN32_CommandLineAppRunner_H_

#include <bdn/GenericAppRunner.h>
#include <bdn/win32/util.h>

namespace bdn
{
namespace win32
{
    
    
class CommandLineAppRunner : public GenericAppRunner
{
public:
	CommandLineAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[])
		: GenericAppRunner( appControllerCreator, makeAppLaunchInfo() )
	{
	}
	

protected:
	void platformSpecificInit() override;
};
  		

    
}    
}

#endif

