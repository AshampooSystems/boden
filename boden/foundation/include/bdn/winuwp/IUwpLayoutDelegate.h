#ifndef BDN_WINUWP_IUwpLayoutDelegate_H_
#define BDN_WINUWP_IUwpLayoutDelegate_H_

#include <bdn/Size.h>
#include <bdn/View.h>
#include <bdn/winuwp/IUwpViewCore.h>
#include <bdn/winuwp/util.h>

namespace bdn
{
namespace winuwp
{

/** Interface for a layout delegate that provides the layout implementation for a UWP
    UIElement.*/
class IUwpLayoutDelegate : BDN_IMPLEMENTS IWeakReferencable
{
public:

    /** Implementation for the MeasureOverride UWP function.

        The caller should use UwpLayoutBridge::doMeasure() instead of calling this directly.
	*/
    virtual Size uwpMeasureOverride(const Size& availableSize )=0;


    /** Implementation for the ArrangeOverride UWP function.	

        Most implementation will simply want to call defaultArrangeOverride() here.

        The caller should use UwpLayoutBridge::doArrange() instead of calling this directly.
	*/
    virtual Size uwpArrangeOverride(const Size& finalSize )=0;


    static Size defaultArrangeOverride(View* pView, const Size& finalSize)
    {
        // The layout was already done at the end of the measure phase.
		// All we need to do here is call Arrange on our child views to activate the layout changes.

		List< P<View> > childViews;
		pView->getChildViews( childViews );

		for(auto& pChildView: childViews)
		{
			P<IUwpViewCore> pChildCore = tryCast<IUwpViewCore>( pChildView->getViewCore() );

			if(pChildCore!=nullptr)
			{
				::Windows::UI::Xaml::FrameworkElement^ pChildElement = pChildCore->getFrameworkElement();

				if(pChildElement!=nullptr)
				{
					Rect						childBounds(pChildView->position(), pChildView->size());
					::Windows::Foundation::Rect winChildBounds = rectToUwpRect( childBounds );

                    ::Windows::Foundation::Size desiredSize = pChildElement->DesiredSize;
				
					pChildElement->Arrange(winChildBounds);
				}
			}
		}		

        return finalSize;
    }

	/** Finalizes the measure data step.

		This is called by the UWP layout bridge (see UwpLayoutBridge)
        at the end of the Measure cycle on the highest level view object that is involved in the layout cycle.

        Most implementation will simply want to call defaultFinalizeUwpMeasure() here.
        
        Custom implementation should do the following:
	
		The view should finalize the layout of direct and indirect child objects in this call.
		The core should assume that the availableSpace parameter from its last Measure
		call will be its final size (see doc_input/winuwp_layout.md for more information
		on why this is correct).
		
		This function is called by UwpLayoutBridge and should usually
		not be called manually.		

		\param lastMeasureAvailableSpace the availableSpace parameter of the last
			measure call.
		*/
	virtual void finalizeUwpMeasure(const Size& lastMeasureAvailableSpace)=0;



    /** Default implementation for finalizeUwpMeasure.
        
        \param pView the view that is being finalized.
        \param lastMeasureAvailableSpace the available space parameter that was passed
            to the last Measure call.
            This size must not have any infinite components. Normally the last measure
            call always specifies a final size (the size that the UI element will get in the
            end), but in rare cases the size can have infinite values in them.
            For example, the content view of a ScrollViewer can get infinite available space
            during measure.

            The caller of defaultFinalizeUwpMeasure must ensure that the
            lastMeasureAvailableSpace does not contain an infinite value. If an infinite value
            exists then the caller should replace this with the corresponding component of
            the UI element's DesiredSize.
    */
    static void defaultFinalizeUwpMeasure(View* pView, const Size& lastMeasureAvailableSpace)
    {        
        // change the View::size() property to reflect the available space of the last measure call.
        // See doc_input/winuwp_layout.md for information on why this works.

        if( !std::isfinite(lastMeasureAvailableSpace.width) || !std::isfinite(lastMeasureAvailableSpace.height) )
            throw InvalidArgumentError( "defaultFinalizeUwpMeasure for "+String( typeid(*pView).name() )+" object was called with an infinite size: "+std::to_string(lastMeasureAvailableSpace.width)+" x "+std::to_string(lastMeasureAvailableSpace.height) );

        Rect newBounds( pView->position(), lastMeasureAvailableSpace);

        pView->adjustAndSetBounds(newBounds);

        // then call layout. This will arrange the child windows and assign them
        // their final sizes and positions. Note that these changes do not modify the sizes
        // and positions of the UWP UI elements. The changes only change the outer view properties.
        // The new values will be transferred to the UWP elements during the Arrange call.

        // Note that it is necessary to go through the whole
        // tree because our layout calls are not recursive. Each call only lays out
        // its direct children.
        layoutViewTree( pView );
            
        
        // Note that adjustAndSetBounds (called during layout for all children in the tree)
        // will automatically call Measure on the framework element. So it is ensured that
        // Measure will be called on all children - which is important because windows will
        // ignore later Arrange calls if the views have not had Measure called on them in
        // this layout cycle.
    }

};

}
}

#endif


