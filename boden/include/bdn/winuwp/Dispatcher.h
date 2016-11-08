#ifndef BDN_WINUWP_Dispatcher_H_
#define BDN_WINUWP_Dispatcher_H_

#include <bdn/IDispatcher.h>

namespace bdn
{
namespace winuwp
{


/** IDispatcher implementation that uses the UWP class Windows.UI.Core.CoreDispatcher.*/
class Dispatcher : public Base, BDN_IMPLEMENTS IDispatcher
{
public:
	Dispatcher(Windows::UI::Core::CoreDispatcher^ pCoreDispatcher)
	{
        _pCoreDispatcher = pCoreDispatcher;
    }


    Windows::UI::Core::CoreDispatcher^ getCoreDispatcher() const
    {
        return _pCoreDispatcher;
    }

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


    /** Disposes the dispatcher and ensures that all pending items are released (but not executed).*/
    void dispose();
    
private:
    template<typename FuncReturnType>
    struct TimedItem : public Base
    {   
        std::function< FuncReturnType() >   func;
        IDispatcher::Priority               priority;
    };

    class Timer : public Base
    {
    public:
        Timer( std::function<bool()> func);

        void dispose();
    };

    void enqueueAndReleaseTimedItemFromOtherThread( TimedItem& item );

    Windows::UI::Core::CoreDispatcher^ _pCoreDispatcher;
};


}
}


#endif
