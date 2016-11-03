#ifndef BDN_WIN32_UiAppRunner_H_
#define BDN_WIN32_UiAppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/IDispatcher.h>

#include <bdn/win32/util.h>
#include <bdn/win32/MessageWindowBase>

namespace bdn
{
namespace win32
{
    
    
class UiAppRunner : public AppRunnerBase, BDN_IMPLEMENTS IDispatcher
{
public:
	UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int showCommand);
    
	int entry();

	void initiateExitIfPossible(int exitCode) override;

    P<IDispatcher> getMainDispatcher() override
    {
        return this;
    }


    // IDispatcher methods
    
	void enqueue(
		std::function< void() > func,
		Priority priority = Priority::normal ) override;
    	
	void enqueueInSeconds(
		double seconds,
		std::function< void() > func,
		Priority priority = Priority::normal ) override;


	void createTimer(
		double intervalSeconds,
		std::function< bool() > func ) override;
    	
	
protected:
	void platformSpecificInit() override;

	void mainLoop() override;

private:

    enum class Message
    {
        /** Handle the next normal priority item*/
        handleNormal,

        /** Do nothing - used to wake up the message queue.*/
        nop,

        executeEndMarker
    };

    class AppMessageWindow : public MessageWindowBase
    {
    public:
        AppMessageWindow(UiAppRunner* pAppRunnerWeak)
        {
            _pAppRunnerWeak = pAppRunnerWeak;
        }
                
	    enum class Message
	    {
		    messageProcess = WM_USER+100
	    };

	    virtual void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
        {
            // we know that the app runner object still exists because
            // it implements the message loop
            _pAppRunnerWeak->handleAppMessage(context, message, wParam, lParam);
        }

    private:
        UiAppRunner* _pAppRunnerWeak;
    };
    friend class AppMessageWindow;

    void handleAppMessage(MessageContext& context, HWND windowHandle,  UINT message, WPARAM wParam, LPARAM lParam);
    
	int _exitCode = 0;
};
  		

    
}    
}

#endif

