#ifndef BDN_WIN32_UiAppRunner_H_
#define BDN_WIN32_UiAppRunner_H_

#include <bdn/AppRunnerBase.h>

#include <bdn/win32/util.h>

namespace bdn
{
namespace win32
{
    
    
class UiAppRunner : public AppRunnerBase
{
public:
	UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int showCommand);


	int entry();

	void initiateExitIfPossible(int exitCode) override;

    P<IDispatcher> getMainDispatcher() override
    {
        return _pMainDispatcher;
    }

	
protected:
	void platformSpecificInit() override;

	void mainLoop() override;
    
	int _exitCode = 0;

    P<IDispatcher> pMainDispatcher;
};
  		

    
}    
}

#endif

