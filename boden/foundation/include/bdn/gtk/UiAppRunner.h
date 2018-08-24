#ifndef BDN_GTK_UiAppRunner_H_
#define BDN_GTK_UiAppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/gtk/MainDispatcher.h>

namespace bdn
{
namespace gtk
{
    
/** AppRunner implementation for GTK apps with a graphical user interface.
	*/
class UiAppRunner : public AppRunnerBase
{
private:
	AppLaunchInfo _makeLaunchInfo(int argCount, char* args[] );

public:
	UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[] );

    bool isCommandLineApp() const override;

	void initiateExitIfPossible(int exitCode) override;

	int entry();

    P<IDispatcher> getMainDispatcher() override
    {
        return _pMainDispatcher;
    }

protected:
    void disposeMainDispatcher() override;
	

	mutable Mutex       _exitMutex;
	int	                _exitCode = 0;
    
    P<MainDispatcher>   _pMainDispatcher;
};
  		

    
}
}

#endif

