#ifndef BDN_WEBEMS_ScrollViewCore_H_
#define BDN_WEBEMS_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>
#include <bdn/webems/ViewCore.h>


namespace bdn
{
namespace webems
{

class ScrollViewCore : public ViewCore, BDN_IMPLEMENTS IScrollViewCore
{
public:
	ScrollViewCore(	ScrollView* pOuter)
		: ViewCore(pOuter, "div" )
	{
        emscripten::val styleObj = _domObject["style"];

        styleObj.set("overflow", "auto");

        // we need a separate "scrolled area" inside the scroll view so that we can
        // force the scrolled area to have a certain size and so that we can potentially
        // clip the content view when needed.
        // Note that we cannot enable scrolling separately for vertical and horizontal
        // because the overflow-x and overflow-y properties are relatively new (CSS 3) and,
        // at the time of this writing, buggy. In Chrome setting overflow-x: scroll will cause
        // the div to scroll in BOTH directions (even if overflow-y is set to hidden).
        // So because of that we enable scrolling for both directions and then
        // use the scrolled area element inside to clip the content if we do not want to scroll
        // in one direction (or in both directions).

		emscripten::val docVal = emscripten::val::global("document");
        _scrolledAreaElementId = "bdn_scrolledarea_"+std::to_string(IdGen::newID());        
        _scrolledAreaDomObject = docVal.call<emscripten::val>("createElement", elementName.asUtf8() );

        _scrolledAreaDomObject.set("id", _scrolledAreaElementId.asUtf8() );

        // we always use absolute positioning. Set this here.
        emscripten::val scrolledAreaStyleObj = _scrolledAreaDomObject["style"];
        scrolledAreaStyleObj.set("position", "absolute");
        // we want the padding to be included in the reported box size.
        scrolledAreaStyleObj.set("box-sizing", "border-box");

		_domObject.call<void>("appendChild", _scrolledAreaDomObject);
	}



	void setHorizontalScrollingEnabled(const bool& enabled) override
	{
		// nothing to do - we fetch this directly from the outer window during layout
	}

	void setVerticalScrollingEnabled(const bool& enabled) override
	{
		// nothing to do - we fetch this directly from the outer window during layout
	}



	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );
		if(pOuter!=nullptr)
		{
			double vertBarWidth;
			double horzBarHeight;
			getScrollBarLayoutSpaces(vertBarWidth, horzBarHeight)

			ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);

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
			double vertBarWidth;
			double horzBarHeight;
			getScrollBarLayoutSpaces(vertBarWidth, horzBarHeight)

			ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);

			
			Size scrollViewSize = pOuterView->size();
			helper.calcLayout( pOuterView, scrollViewSize );

			// resize the scrolled area
			_scrolledAreaSize = helper.getScrolledAreaSize();
			setDomScrolledAreaSize(_scrolledAreaSize);


			// now arrange the content view inside the scrolled area
			Rect contentBounds = helper.getContentViewBounds();

			P<View> pContentView = pOuterView->getContentView();
			if(pContentView!=nullptr)
				pContentView->adjustAndSetBounds( contentBounds );
		}
	}

	/** Returns the amount of space that the scroll bars take up when scrolling
		is done.
		If the scroll bars are overlays and do not influence the layout then the
		corresponding value will be set to 0.*/
	void getScrollBarLayoutSpaces(double& vertBarWidth, double& horzBarHeight)
	{
		P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
		if(pOuterView!=nullptr)
		{
			Size scrollViewSize = pOuterView->size();

			// first we temporarily give the scrolled area a size that exceeds the
			// viewport size to force the scrollbars to appear
			setDomScrolledAreaSize( scrollViewSize+Size(100,100) );
			

        	// at this point the scroll bars are there and we can subtract the client size from the view size
        	
        	// Note that we use getBoundingClientRect to get an accurate size that was not rounded to integers.
        	emscripten::val rectObj = _domObject.call<emscripten::val>("getBoundingClientRect");

        	double clientWidth = rectObj["width"].as<double>();
        	double clientHeight = rectObj["height"].as<double>();

        	vertBarWidth = ScrollViewSize.width - clientWidth;
        	horzBarHeight = ScrollViewSize.height - clientHeight;

        	if(vertBarWidth<0)
        		vertBarWidth = 0;
        	if(horzBarHeight<0)
        		horzBarHeight = 0;

        	// set the scrolled area size back to what it was
        	setDomScrolledAreaSize( _scrolledAreaSize );
        }
        else
        {
        	vertBarWidth = 0;
        	horzBarHeight = 0;
        }
	}

private:

	void setDomScrolledAreaSize(const Size& size)
	{
		emscripten::val scrolledAreaStyleObj = _scrolledAreaDomObject["style"];
    	scrolledAreaStyleObj.set("width", std::to_string((int)std::lround(size.width))+"px");
    	scrolledAreaStyleObj.set("height", std::to_string((int)std::lround(size.height))+"px");

	}

	

	Size _scrolledAreaSize;
};

}
}

#endif
