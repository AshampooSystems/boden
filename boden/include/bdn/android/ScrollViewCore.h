#ifndef BDN_ANDROID_ScrollViewCore_H_
#define BDN_ANDROID_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>
#include <bdn/ScrollViewLayoutHelper.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/JScrollView.h>
#include <bdn/android/JHorizontalScrollView.h>
#include <bdn/android/IParentViewCore.h>
#include <bdn/android/JViewGroup.h>


namespace bdn
{
namespace android
{

class ScrollViewCore : public ViewCore, BDN_IMPLEMENTS IScrollViewCore, BDN_IMPLEMENTS IParentViewCore
{
private:
	static P<JScrollView> _createJScrollView(ScrollView* pOuter)
	{
		// we need to know the context to create the view.
		// If we have a parent then we can get that from the parent's core.
		P<View> 	pParent = pOuter->getParentView();
		if(pParent==nullptr)
			throw ProgrammingError("ScrollViewCore instance requested for a ScrollView that does not have a parent.");

		P<ViewCore> pParentCore = cast<ViewCore>( pParent->getViewCore() );
		if(pParentCore==nullptr)
			throw ProgrammingError("ScrollViewCore instance requested for a ScrollView with core-less parent.");

		JContext context = pParentCore->getJView().getContext();

		P<JScrollView> pVertScrollView = newObj<JScrollView>(context);

		return pVertScrollView;
	}

public:
	ScrollViewCore(ScrollView* pOuter)
     : ViewCore(pOuter, _createJScrollView(pOuter) )
	{
		_pVertScrollView = newObj<JScrollView>( getJView().getRef_() );

		// unfortunately android does not offer a scroll view that can scroll in both directions.
		// So we emulate this by nesting a horizontal scroll view inside the vertical scroll view.
		_pHorzScrollView = newObj<JHorizontalScrollView>( _pVertScrollView->getContext() );

		_pVertScrollView->addView( *_pHorzScrollView );

		// inside the scroll view we add a NativeViewGroup object as the glue between our layout system and
		// that of android. That allows us to position the content view manually. It also ensures that
		// the parent of the content view is a NativeViewGroup, which is important because we assume
		// that that is the case in some places.
		_pContentParent = newObj<JNativeViewGroup>( _pVertScrollView->getContext() );
		_pHorzScrollView->addView( *_pContentParent );

		setVerticalScrollingEnabled( pOuter->verticalScrollingEnabled() );
		setHorizontalScrollingEnabled( pOuter->horizontalScrollingEnabled() );
	}



	void setHorizontalScrollingEnabled(const bool& enabled) override
	{
		// nothing to do - we get this directly from the outer window
	}

	void setVerticalScrollingEnabled(const bool& enabled) override
	{
		// nothing to do - we get this directly from the outer window
	}


	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );
		if(pOuter!=nullptr)
		{
			// note that the scroll bars are overlays and do not take up any layout space.
			ScrollViewLayoutHelper helper(0, 0);

			return helper.calcPreferredSize(pOuter, availableSpace);
		}
		else
			return Size(0,0);
	}

	void layout() override
	{
		P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
		if(pOuterView!=nullptr)
		{
			// note that the scroll bars are overlays and do not take up any layout space.
			ScrollViewLayoutHelper helper(0, 0);

			Size scrollViewSize = pOuterView->size();

			helper.calcLayout( pOuterView, scrollViewSize );

			Size scrolledAreaSize = helper.getScrolledAreaSize();

			double uiScaleFactor = getUiScaleFactor();

			// resize the content parent to the scrolled area size.
			// That causes the content parent to get that size the next time
			// and android layout happens.
			_pContentParent->setSize( std::lround( scrolledAreaSize.width * uiScaleFactor),
									std::lround( scrolledAreaSize.height * uiScaleFactor)
									);

			// now arrange the content view inside the content parent
			Rect contentBounds = helper.getContentViewBounds();

			P<View> pContentView = pOuterView->getContentView();
			if(pContentView!=nullptr)
				pContentView->adjustAndSetBounds( contentBounds );

			// we must call _pContentParent->requestLayout because we have to clear
			// its measure cache. Otherwise the changes might not take effect.
			_pContentParent->requestLayout();
		}
	}


	double getUiScaleFactor() const override
	{
		return ViewCore::getUiScaleFactor();
	}

	void addChildJView( JView childJView ) override
	{
		if( !_currContentJView.isNull_() )
			_pContentParent->removeView( _currContentJView );

		_currContentJView = childJView;
		_pContentParent->addView( childJView );
	}


private:
	P<JScrollView>		  	 _pVertScrollView;
	P<JHorizontalScrollView> _pHorzScrollView;
	P<JNativeViewGroup>      _pContentParent;

	JView				     _currContentJView;
};

}
}

#endif
