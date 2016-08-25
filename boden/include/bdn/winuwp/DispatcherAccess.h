#ifndef BDN_WINUWP_DispatcherAccess_H_
#define BDN_WINUWP_DispatcherAccess_H_

#include <bdn/winuwp/platformError.h>

namespace bdn
{
namespace winuwp
{


/** Enables easy access to CoreDispatcher instances.*/
class DispatcherAccess : public Base
{
public:
	DispatcherAccess()
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pMainDispatcher = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher;

        BDN_WINUWP_TO_STDEXC_END;
    }

    Windows::UI::Core::CoreDispatcher^ getMainDispatcher()
    {
        return _pMainDispatcher;
    }

    BDN_SAFE_STATIC( DispatcherAccess, get );

private:
    Windows::UI::Core::CoreDispatcher^ _pMainDispatcher;
};

}
}


#endif
