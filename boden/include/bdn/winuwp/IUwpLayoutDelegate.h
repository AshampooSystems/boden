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

        The caller should use instantiate UwpMeasureFinalizer BEFORE calling this function
        to ensure that finalizeUwpMeasure is called automatically.
	*/
    virtual Size uwpMeasureOverride(const Size& availableSize )=0;


    /** Implementation for the ArrangeOverride UWP function.	

        Most implementation will simply want to call defaultArrangeOverride() here.
	*/
    virtual Size uwpArrangeOverride(const Size& finalSize )=0;


    static Size defaultArrangeOverride(View* pView, const Size& finalSize)
    {
        // The layout was already done at the end of the measure phase.
		// All we need to do here is call Arrange on our child views to activate the layout changes.

		std::list< P<View> > childViews;
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

		If the measure function implementation uses UwpMeasureFinalizer then
		zhis is called at the end of the Measure cycle on
		the highest level view that is involved in the layout cycle.

        Most implementation will simply want to call defaultFinalizeUwpMeasure() here.
        
        Custom implementation should do the following:
	
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



    /** Default implementation for finalizeUwpMeasure.
        
        \param pView the view that is being finalized.
        \param lastMeasureAvailableSpace the available space parameter that was passed
            to the last Measure call.
    */
    static void defaultFinalizeUwpMeasure(View* pView, const Size& lastMeasureAvailableSpace)
    {        
        // change the View::size() property to reflect the available space of the last measure call.
        // See doc_input/winuwp_layout.md for information on why this works.

        Rect newBounds( pView->position(), lastMeasureAvailableSpace);

        pView->adjustAndSetBounds(newBounds);
            
        // then call layout. This will arrange the child windows and assign them
        // their final sizes and positions. Note that these changes do not modify the sizes
        // and positions of the UWP UI elements. The changes only change the outer view properties.
        // The new values will be transferred to the UWP elements during the Arrange call.
        P<IViewCore> pCore = pView->getViewCore();
        if(pCore!=nullptr)
            pCore->layout();
        
        // Note that adjustAndSetBounds (called during layout for all children in the tree)
        // will automatically call Measure on the framework element. So it is ensured that
        // Measure will be called on all children - which is important because windows will
        // ignore later Arrange calls if the views have not had Measure called on them in
        // this layout cycle.
    }

	
	/** Helper class that calls finalizeUwpMeasure when the top level Measure call finishes.

        The finalizer should be instantiated by the CALLER of IUwpLayoutDelegate::uwpMeasureOverride.
        The IUwpLayoutDelegate implementations themselves do not use UwpMeasureFinalizer.
        
		The finalizer objects track how many recursive measure calls are running.

        You should call finalizeIfTopLevel() at the end of the measure call. It will call
        call finalizeUwpMeasure if the Measure call is at the top level (i.e. if the finalizer
        object is the only one that still exists).
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

        /** This should be called at the end of the Measure call.
            It calls finalizeUwpMeasure on the delegate if the current finalizer is the
            only one that exists (i.e. if the current Measure call is at the top level).           
            
            */
        void finalizeIfTopLevel()
        {
            // Implementation note: One might think that finalizeIfTopLevel could be called
            // automatically from the finalizer destructor. While that would work in most cases,
            // it does not work if the finalizeUwpMeasure function throws an exception (since
            // destructors should not throw any exceptions).
            // So to avoid this problem we do not do this stuff in the destructor but in a separate
            // function.

            if(_activeFinalizers==1)
            {            
                // measure calls that occur during finalize should not trigger another
                // finalization. We set _activeFinalizers to a dummy number to prevent that.
                _activeFinalizers = 0x10000000;

                OutputDebugString( (String(typeid(*_pDelegate).name()) +" finalizeUwpMeasure("+std::to_string(_measureAvailableSpace.width)+"x"+std::to_string(_measureAvailableSpace.height)+")").asWidePtr() );

                try
                {
				    _pDelegate->finalizeUwpMeasure(_measureAvailableSpace);
                }
                catch(...)
                {
                    _activeFinalizers = 1;
                    throw;
                }

                OutputDebugString( ("/"+String(typeid(*_pDelegate).name()) +" finalizeUwpMeasure()\n").asWidePtr() );

                _activeFinalizers = 1;
            }
        }

		~UwpMeasureFinalizer()
		{
			_activeFinalizers--;

			
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


