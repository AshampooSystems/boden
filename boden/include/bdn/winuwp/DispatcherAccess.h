#ifndef BDN_WINUWP_DispatcherAccess_H_
#define BDN_WINUWP_DispatcherAccess_H_

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
        _pMainDispatcher = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher;
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
