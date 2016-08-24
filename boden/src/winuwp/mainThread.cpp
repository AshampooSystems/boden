#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/winuwp/DispatcherAccess.h>

namespace bdn
{	

void CallFromMainThreadBase_::dispatch()
{
    BDN_WINUWP_TO_STDEXC_BEGIN;

	P<ISimpleCallable> pThis = this;


	bdn::winuwp::DispatcherAccess::get().getMainDispatcher()->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Normal,
		ref new Windows::UI::Core::DispatchedHandler(
			[pThis]()
			{
				pThis->call();
			} ) );


    BDN_WINUWP_TO_STDEXC_END;

}


}

