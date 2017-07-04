#ifndef BDN_WINUWP_UwpPanelWithCustomLayout_H_
#define BDN_WINUWP_UwpPanelWithCustomLayout_H_

#include <bdn/winuwp/IUwpLayoutDelegate.h>

namespace bdn
{
namespace winuwp
{


/** UWP panel class that delegates layout functionality to an external object.

	Use setLayoutDelegateWeak to set the delegate.
*/
public ref class UwpPanelWithCustomLayout sealed : public ::Windows::UI::Xaml::Controls::Panel
{
internal:
    
    UwpPanelWithCustomLayout( )
    {
    }


	/** Sets the layout delegate that this panel forwards its layout calls to.

		The panel only holds a weak reference to the delegate. I.e. it does not keep the delegate alive.
		When the delegate is deleted then the layout functions will fall back to default implementations
		(returning a zero desired size and doing nothing in arrange).
	*/
	void setLayoutDelegateWeak(IUwpLayoutDelegate* pDelegate)
	{
		_layoutDelegateWeak = pDelegate;
	}

protected:
    ::Windows::Foundation::Size MeasureOverride(::Windows::Foundation::Size availableSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

		P<IUwpLayoutDelegate> pDelegate = _layoutDelegateWeak.toStrong();
		if(pDelegate!=nullptr)
			return pDelegate->measureOverride(this, availableSize);
		else
			return ::Windows::Foundation::Size(0,0);

        BDN_WINUWP_TO_PLATFORMEXC_END;
    }


    ::Windows::Foundation::Size ArrangeOverride(::Windows::Foundation::Size finalSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

		P<IUwpLayoutDelegate> pDelegate = _layoutDelegateWeak.toStrong();
		if(pDelegate!=nullptr)
			return pDelegate->arrangeOverride(this, finalSize);
		else
			return ::Windows::Foundation::Size(0,0);
		
        BDN_WINUWP_TO_PLATFORMEXC_END;
    }


private:
    WeakP<IUwpLayoutDelegate> _layoutDelegateWeak;
};



}
}

#endif
