#ifndef BDN_WINUWP_UwpPanelWithCustomLayout_H_
#define BDN_WINUWP_UwpPanelWithCustomLayout_H_

#include <bdn/winuwp/IUwpLayoutDelegate.h>

namespace bdn
{
namespace winuwp
{


/** Helper class to create instances of UwpPanelWithCustomLayout.*/
class UwpPanelWithCustomLayoutFactory
{
public:
    

    /** Creates an instance of UwpPanelWithCustomLayout with the specified layout delegate.*/
    static UwpPanelWithCustomLayout^ createInstance( IUwpLayoutDelegate* pDelegate )
    {
        return ref new UwpPanelWithCustomLayout( pDelegate );
    }
};


/** UWP panel class that delegates layout functionality to an external object.

    Use the UwpPanelWithCustomLayoutFactory::createInstance() static function to create an instance of this class.
*/
ref class UwpPanelWithCustomLayout sealed : public ::Windows::UI::Xaml::Controls::Canvas
{
protected:
    /** This constructor has to be protected because it uses non-UWP classes. Use UwpPanelWithCustomLayoutFactory::createInstance
        to create an instance of this class.
    
        \param measureDelegate the function that is called for the MeasureOverride UWP function.
        \param arrangeDelegate the function that is called for the ArrangeOverride UWP function.*/
    UwpPanelWithCustomLayout( IUwpLayoutDelegate* pLayoutDelegate )
    {
        _pLayoutDelegate = pLayoutDelegate;
    }

public:
    /** Creates an instance of UwpPanelWithCustomLayout with the specified layout delegate.*/
    static UwpPanelWithCustomLayout^ createInstance( IUwpLayoutDelegate* pDelegate )
    {
        return ref new UwpPanelWithCustomLayout( pDelegate );
    }
    

protected:
    ::Windows::Foundation::Size MeasureOverride(::Windows::Foundation::Size availableSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

        return _pLayoutDelegate->measureOverride(availableSize);

        BDN_WINUWP_TO_PLATFORMEXC_END;
    }


    ::Windows::Foundation::Size ArrangeOverride(::Windows::Foundation::Size finalSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

        return _pLayoutDelegate->arrangeOverride(availableSize);

        BDN_WINUWP_TO_PLATFORMEXC_END;
    }


private:
    P<IUwpLayoutDelegate> _pLayoutDelegate;
};


}
}

#endif
