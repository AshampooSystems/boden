#ifndef BDN_WINUWP_UwpPanelWithCustomLayout_H_
#define BDN_WINUWP_UwpPanelWithCustomLayout_H_

#include <bdn/winuwp/IUwpLayoutDelegate.h>

namespace bdn
{
namespace winuwp
{


/** UWP panel class that delegates layout functionality to an external object.

    Use the UwpPanelWithCustomLayoutFactory::createInstance() static function to create an instance of this class.
*/
public ref class UwpPanelWithCustomLayout sealed : public ::Windows::UI::Xaml::Controls::Panel
{
internal:
    /** This constructor has to be protected because it uses non-UWP classes. Use UwpPanelWithCustomLayoutFactory::createInstance
        to create an instance of this class.
    
        \param measureDelegate the function that is called for the MeasureOverride UWP function.
        \param arrangeDelegate the function that is called for the ArrangeOverride UWP function.*/
    UwpPanelWithCustomLayout( IUwpLayoutDelegate* pLayoutDelegate )
    {
        _pLayoutDelegate = pLayoutDelegate;
    }

protected:
    ::Windows::Foundation::Size MeasureOverride(::Windows::Foundation::Size availableSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

        return _pLayoutDelegate->measureOverride(this, availableSize);

        BDN_WINUWP_TO_PLATFORMEXC_END;
    }


    ::Windows::Foundation::Size ArrangeOverride(::Windows::Foundation::Size finalSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

        return _pLayoutDelegate->arrangeOverride(this, finalSize);

        BDN_WINUWP_TO_PLATFORMEXC_END;
    }


private:
    P<IUwpLayoutDelegate> _pLayoutDelegate;
};



}
}

#endif
