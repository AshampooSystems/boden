#ifndef BDN_WINUWP_UwpViewWithLayoutDelegate_H_
#define BDN_WINUWP_UwpViewWithLayoutDelegate_H_


namespace bdn
{
namespace winuwp
{


/** Template class that overrides the layout functions (MeasureOverride and ArrangeOverride) of the
    base class and redirects these calls to a layout delegate object.

	Use setLayoutDelegateWeak to set the delegate.
    */
template<class BaseClass = ::Windows::UI::Xaml::Controls::Panel>
private ref class UwpViewWithLayoutDelegate : public BaseClass
{
internal:    
    UwpViewWithLayoutDelegate( )
    {
    }


	/** Sets the layout delegate that this panel forwards its layout calls to.

		The UWP view only holds a weak reference to the delegate. I.e. it does not keep the delegate alive.
		When the delegate is deleted then the layout functions will fall back to default implementations
		(returning a zero desired size and doing nothing in arrange).
	*/
	void setLayoutDelegateWeak(IUwpLayoutDelegate* pDelegate)
	{
		_layoutDelegateWeak = pDelegate;
	}




protected:
   
    ::Windows::Foundation::Size MeasureOverride(::Windows::Foundation::Size winAvailableSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;
               
		P<IUwpLayoutDelegate> pDelegate = _layoutDelegateWeak.toStrong();
		if(pDelegate!=nullptr)
        {
            Size availableSpace = uwpSizeToSize(winAvailableSize);

            IUwpLayoutDelegate::UwpMeasureFinalizer finalizer(pDelegate, availableSpace);

			Size resultSize = pDelegate->measureOverride(this, availableSpace);

            return sizeToUwpSize(resultSize);
        }
		else
			return ::Windows::Foundation::Size(0,0);

        BDN_WINUWP_TO_PLATFORMEXC_END;
    }


    ::Windows::Foundation::Size ArrangeOverride(::Windows::Foundation::Size winFinalSize) override
    {
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

		P<IUwpLayoutDelegate> pDelegate = _layoutDelegateWeak.toStrong();
		if(pDelegate!=nullptr)
        {
            Size finalSize = uwpSizeToSize(winFinalSize);
        
			Size resultSize = pDelegate->arrangeOverride(this, finalSize);

            return uwpSizeToSize(resultSize);
        }
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
