#ifndef BDN_WINUWP_ContainerViewCore_H_
#define BDN_WINUWP_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/winuwp/ChildViewCore.h>
#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/UwpViewWithLayoutDelegate.h>

namespace bdn
{
namespace winuwp
{

class ContainerViewCore : public ChildViewCore, BDN_IMPLEMENTS IUwpLayoutDelegate
{

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ChildViewCore(
            pOuter,
            ref new UwpViewWithLayoutDelegate<>(),
            ref new ViewCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// note that 
		_pUwpView = dynamic_cast< UwpViewWithLayoutDelegate<>^ >( getFrameworkElement() );

		// it is ok to set this as the delegate, since the panel only holds a weak reference
		// to the delegate. I.e. there will not be a reference cycle.
		_pUwpView->setLayoutDelegateWeak(this);
        
        BDN_WINUWP_TO_STDEXC_END;
	}


	void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement ) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pUwpView->Children->Append( pUiElement );		

        BDN_WINUWP_TO_STDEXC_END;        
	}


	void setPadding(const Nullable<UiMargin>& uiPadding) override
	{
		// the outer class automatically handles our own padding. So nothing to do here.
	}
    		
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}



	
    Size uwpMeasureOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, const Size& availableSpace ) override
	{
		P<View> pOuterView = getOuterViewIfStillAttached();

		if(pOuterView!=nullptr)
		{
			XXX
			update sizing info on child views

			// first calculate our preferred size for the specified amount of available space.
			// Note that this will sometimes call Measure on some child views, but that is not guaranteed.
			// It depends on the container implementation, as the container might also simply use the cached
			// View::preferredSize() property.

            // Because of this issue we need to explicitly call measure on all our child views to ensure
            // that windows will recognize that they are part of the layout. Otherwise we won't be able to
            // arrange them afterwards.
                XXX

			Size preferredSize = pOuterView->calcPreferredSize(availableSpace); 

			return sizeToUwpSize(preferredSize);
		}
		else
			return ::Windows::Foundation::Size(0,0);
	}

    void finalizeUwpMeasure(const Size& lastMeasureAvailableSpace) override
    {
        // size ourselves according to the last measure call. See doc_input/winuwp_layout.md for more information
        // about this.

        P<View> pOuterView = getOuterViewIfStillAttached();

		if(pOuterView!=nullptr)
        {
            Rect newBounds( pOuterView->position(), lastMeasureAvailableSpace);

            pOuterView->adjustAndSetBounds(newBounds);
            
            // then layout to ensure that the child windows are sized correctly.
            pOuterView->layout();
        }
    }


    /** Implementation for the ArrangeOverride UWP function.*/
    Size uwpArrangeOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, const Size& finalSize ) override
	{
		// The layout was already done at the end of the measure phase.
		// All we need to do here is call Arrange on our child views to activate the layout changes.

		P<View> pOuterView = getOuterViewIfStillAttached();

		std::list< P<View> > childViews;
		pOuterView->getChildViews( childViews );

		for(auto& pChildView: childViews)
		{
			P<ChildViewCore> pChildCore = tryCast<ChildViewCore>( pChildView->getViewCore() );

			if(pChildCore!=nullptr)
			{
				::Windows::UI::Xaml::FrameworkElement^ pChildElement = pChildCore->getFrameworkElement();

				if(pChildElement!=nullptr)
				{
					Rect						childBounds(pChildView->position(), pChildView->size());
					::Windows::Foundation::Rect winChildBounds = rectToUwpRect( childBounds );
				
					pChildElement->Arrange(winChildBounds);
				}
			}
		}		
	}
	

	void finalizeUwpMeasure() override
	{
		// this is called at the end of the UWP measure operation
		// (when the Measure call of the top level parent finishes, i.e.
		// just before we enter the Arrange phase.

		// We get sized according to our DesiredSize.

		// The DesiredSize of a view is the result of the last Measure call.
		// Since DesiredSize also imposes restrictions on the final size a view can have
		// that means that we have to make sure that the DesiredSize is not too big.
		// Also, for sub-containers we have to ensure that the availableSpace from the
		// last measure call matches the final size exactly, otherwise their cached layout
		// might be incorrect.
		// However, this is only necessary if the 
		// So, check that now.

		const std::list< P<ChildViewCore> >&	childCores = getUwpMeasureCores();

		for( auto& pChildCore: childCores)
		{
			P<View> pChildView = pChildCore->getOuterViewIfStillAttached();
			if(pChildView!=nullptr)
			{
				::Windows::UI::Xaml::FrameworkElement^ pChildElement = pChildCore->getFrameworkElement();
				if(pChildElement!=nullptr)
				{
					P<ViewLayout::ViewLayoutData> pData = _pCurrLayout->getViewLayoutData( pChildView );
					if(pData!=nullptr)
					{
						Rect childBounds;
							
						if(pData->getBounds(childBounds) )
						{
							::Windows::Foundation::Size layoutSize = sizeToUwpSize( childBounds.getSize() );

							::Windows::Foundation::Size desiredSize = pChildElement->DesiredSize;

							// DesiredSize must not be bigger than layoutSize, otherwise we the layoutSize
							// will be overruled by Windows. If it is too big then we need to call Measure
							// again on the child and pass its final size as availableSpace - Windows will
							// then make sure that DesiredSize does not exceed it.

							// For sub-containers DesiredSize should also no be smaller than layoutSize, since our sub-containers
							// used that size to arrange their children - and that layout will be used unchanged
							// after we return here.
							
							// So if DesiredSize is at all different from layoutSize then we call measure again.
							// Note that we allow for a tiny difference since we are dealing with floats here and
							// some tiny deviations are to be expected.

							if( fabs(desiredSize.Width - layoutSize.Width) > 0.01
								|| fabs(desiredSize.Height - layoutSize.Height) > 0.01)
							{
								pChildElement->Measure(layoutSize);
							}
						}
					}
				}
			}
		}
	}

	::Windows::Foundation::Size uwpArrange(::Windows::Foundation::Size winFinalSize, ContainerView* pOuterView)
	{
		// just apply the current layout.
		// See doc_input/winuwp_layout.md for an explanation as to why we do not
		// look at the final size here.
		if(_pCurrLayout!=nullptr)
			_pCurrLayout->applyTo(pOuterView);

		return winFinalSize;
	}

			
	    
	UwpViewWithLayoutDelegate<>^ _pUwpView;

	P<ViewLayout>			  _pCurrLayout;
	
};

}
}

#endif
