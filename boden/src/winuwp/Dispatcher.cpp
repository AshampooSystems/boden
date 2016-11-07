#include <bdn/init.h>
#include <bdn/Dispatcher.h>

namespace bdn
{
namespace winuwp
{



void Dispatcher::enqueue(
	std::function< void() > func,
	Priority priority)
{
    BDN_WINUWP_TO_STDEXC_BEGIN;
    
	bdn::winuwp::DispatcherAccess::get().getMainDispatcher()->RunAsync(
		::Windows::UI::Core::CoreDispatcherPriority::Normal,
		ref new Windows::UI::Core::DispatchedHandler(
			[func]()
			{
                BDN_WINUWP_TO_PLATFORMEXC_BEGIN

				func();

                BDN_WINUWP_TO_PLATFORMEXC_END
			} ) );


    BDN_WINUWP_TO_STDEXC_END;
}


void Dispatcher::enqueueInSeconds(
	double seconds,
	std::function< void() > func,
	Priority priority)
{    
    if(seconds<=0.0000001)
    {
        enqueue(func, priority);
        return;
    }

	struct CallData : public Base
    {
        ~CallData()
        {
            int x=0;
            x++;
        }

        P<ISimpleCallable>                      pCallable;
    };

    P<CallData> pCallData = newObj<CallData>();

    pCallData->pCallable = this;

    ::Windows::Foundation::TimeSpan ts;
    ts.Duration = (int64_t)(seconds*10000000);
    
    // note that we would love to use ::Windows::UI::Xaml::DispatcherTimer here,
    // but unfortunately it does not support setting a priority and the default priority
    // is apparently lower than "normal". That means that timer events only occur when
    // the event queue is empty, which is not good enough for us.
    // So instead we are forced to use a ThreadPoolTimer instead.

    ::Windows::System::Threading::ThreadPoolTimer::CreateTimer(
        ref new ::Windows::System::Threading::TimerElapsedHandler(
            [pCallData](::Windows::System::Threading::ThreadPoolTimer^ pTimer)
            {
                BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                // this handler will be called from another thread, so we must
                // redirect it to the main thread.
                callFromMainThread(
                    [pCallData]()
                    {
                        pCallData->pCallable->call();
                        
                        // we MUST set the callable to null here, so that we can be sure that
                        // it is released from the main thread.
                        // Since the timer handler is called from another thread and the handler holds
                        // a reference to pCallData, the pCallData object will be released from the worker
                        // thread. So if pCallable is not nulled here, it will also get released from that thread.
                        // And that might cause strange problems, especially if the callable references
                        // UI objects.
                        pCallData->pCallable = nullptr;
                    } );

                BDN_WINUWP_TO_PLATFORMEXC_END
            } ),
        ts );

}    


void Dispatcher::createTimer(
	double intervalSeconds,
	std::function< bool() > func )
{
}


void Dispatcher::dispose()
{
}
    

}
}


#endif
