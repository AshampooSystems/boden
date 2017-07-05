#ifndef BDN_WINUWP_IUwpLayoutDelegate_H_
#define BDN_WINUWP_IUwpLayoutDelegate_H_

#include <bdn/Size.h>

namespace bdn
{
namespace winuwp
{

/** Interface for a layout delegate that provides the layout implementation for a UWP
    UIElement.*/
class IUwpLayoutDelegate : BDN_IMPLEMENTS IBase
{
public:

    /** Implementation for the MeasureOverride UWP function.

        The caller should use instantiate UwpMeasureFinalizer BEFORE calling this function
        to ensure that finalizeUwpMeasure is called automatically.
	*/
    virtual Size uwpMeasureOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, const Size& availableSize )=0;


    /** Implementation for the ArrangeOverride UWP function.	
	*/
    virtual Size uwpArrangeOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, const Size& finalSize )=0;


	/** Finalizes the measure data step.

		If the measure function implementation uses UwpMeasureFinalizer then
		zhis is called at the end of the Measure cycle on
		the highest level view that is involved in the layout cycle.
	
		The view should finalize the layout of child objects in this call.
		The core should assume that the availableSpace parameter from its last Measure
		call will be its final size (see doc_input/winuwp_layout.md for more information
		on why this is correct).
		
		This function is called by UwpMeasureFinalizer helper objects that should
		be instantated during measure. You should not call it manually.		

		\param lastMeasureAvailableSpace the availableSpace parameter of the last
			measure call.
		*/
	virtual void finalizeUwpMeasure(const Size& lastMeasureAvailableSpace)=0;

	
	/** Helper class that ensures that calls finalizeUwpMeasure
		when the top level Measure call finishes.

        The finalizer should be instantiated by the CALLER of IUwpLayoutDelegate::uwpMeasureOverride.
        The IUwpLayoutDelegate implementations do not use it.
        
		The finalizer objects track how many recursive measure calls are running.
		When the last one finishes at the top level then the finalizers call finalizeUwpMeasure
		on that top level delegate.
		*/
	class UwpMeasureFinalizer
	{
	public:
		UwpMeasureFinalizer(IUwpLayoutDelegate* pDelegate, const Size& measureAvailableSpace)
		{
			_pDelegate = pDelegate;
			_measureAvailableSpace = measureAvailableSpace;

			_activeFinalizers++;
		}

		~UwpMeasureFinalizer()
		{
			_activeFinalizers--;

			if(_activeFinalizers==0)
				_pDelegate->finalizeUwpMeasure(_measureAvailableSpace);
		}

	private:
		static int			_activeFinalizers;
		
		// weak by design
		IUwpLayoutDelegate*	_pDelegate;
		Size				_measureAvailableSpace;
	};

};

}
}

#endif


