#ifndef BDN_WEBEMS_ScrollViewCore_H_
#define BDN_WEBEMS_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>
#include <bdn/ScrollViewLayoutHelper.h>

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
        , _scrolledAreaDomObject( emscripten::val::null() )
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
        _scrolledAreaDomObject = docVal.call<emscripten::val>("createElement", std::string("div") );

        _scrolledAreaDomObject.set("id", _scrolledAreaElementId.asUtf8() );

        // we always use absolute positioning. Set this here.
        emscripten::val scrolledAreaStyleObj = _scrolledAreaDomObject["style"];
        scrolledAreaStyleObj.set("position", "absolute");
        // we want the padding to be included in the reported box size.
        scrolledAreaStyleObj.set("box-sizing", "border-box");

        // it is very important that the scrolled area has "overflow hidden".
        // Otherwise the content of the scrolled area can overflow the size we
        // have set for it and thus it can happen that scroll bars appear when we
        // do not want them to appear (e.g. if scrolling is disabled).
        scrolledAreaStyleObj.set("overflow", "hidden");

		_domObject.call<void>("appendChild", _scrolledAreaDomObject);

        // the ViewCore constructor will have set the padding. Clear that again
        // (see setPadding for more info)
        setDefaultPadding();
	}



	void setHorizontalScrollingEnabled(const bool& enabled) override
	{
		// nothing to do - we fetch this directly from the outer window during layout
	}

	void setVerticalScrollingEnabled(const bool& enabled) override
	{
		// nothing to do - we fetch this directly from the outer window during layout
	}



    void setPadding(const Nullable<UiMargin>& padding) override
    {
        // the ScrollViewLayoutHelper automatically applies the padding.
        // So we must not set it in the DOM element of the outer scroll view.

        // So, do nothing here.
    }


	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );
		if(pOuter!=nullptr)
		{
			double vertBarWidth;
			double horzBarHeight;
			getScrollBarLayoutSpaces(vertBarWidth, horzBarHeight);

			ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);

			Size prefSize = helper.calcPreferredSize(pOuter, availableSpace);

            return prefSize;
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
			getScrollBarLayoutSpaces(vertBarWidth, horzBarHeight);

			ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);

            Size scrollViewSize = pOuterView->size();
			helper.calcLayout( pOuterView, scrollViewSize );

			_scrolledAreaSize = helper.getScrolledAreaSize();

            // unfortunately the layout system of many browsers (chrome for example)
            // is a little bit broken as far as scrolling is concerned.
            // When a scrollbar is shown this reduces the viewport size, since some space
            // is used by the scrollbar.
            // The problem is that if scroll bars are currently visible then they only
            // disappear when the content size becomes smaller than this reduced viewport
            // size. Even if the content would fit completely into the bigger viewport
            // with no scrollbars.
            // For example: content size 100x100, scrollview size 100x100.
            // Obviously no scrollbar is needed.
            // If the content is enlarged to 200x200 then two scrollbars are shown.
            // The viewport becomes smaller (let's say 90x90).
            // When the content now goes back to 100x100 then scrollbars are still shown
            // because the content is bigger than the CURRENT viewport of 90x90.
            // When the content is made even smaller to 90x90 then the scrollbars disappear.
            // At this point one can make the content bigger again to 100x100 without
            // scrollbars appearing.

            // So, to work around this problem we ALWAYS set the scrolled area size to 0x0 at first
            // to hide any scrollbars and then we enlarge it to the actual size.
            setDomScrolledAreaSize( Size(0,0) );

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
	void getScrollBarLayoutSpaces(double& vertBarWidth, double& horzBarHeight) const
	{
        if(_vertScrollBarLayoutWidth==-1)
        { 
    		P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
    		if(pOuterView!=nullptr)
    		{
                // first we must ensure that the scroll view has a certain minimum size, so that
                // it can fully display a scroll bar.
                // especially in the beginning the scrollview size might be (0,0) otherwise.

                emscripten::val styleObj = _domObject["style"];
                emscripten::val oldWidthStyle = styleObj["width"];
                emscripten::val oldHeightStyle = styleObj["height"];
                styleObj.set("width", std::string("400px") );
                styleObj.set("height", std::string("400px") );

    			// first we temporarily give the scrolled area a size that exceeds the
    			// viewport size to force the scrollbars to appear
    			setDomScrolledAreaSize( Size(500,500) );    			

            	// at this point the scroll bars are there and we can subtract the client size from the view size

                // IMPORTANT: we cannot use getBoundingClientRect here. At the time of this writing, getBoundingClientRect
                // does NOT exclude the scrollbar sizes. It should exclude them (according to the documentation), but
                // it does not.

                // We use clientWidth and clientHeight instead. Those are rounded to integer pixels, but we assume that
                // the scroll bars have an integer size anyway.

                double clientWidth = _domObject["clientWidth"].as<double>();
                double clientHeight = _domObject["clientWidth"].as<double>();

            	_vertScrollBarLayoutWidth = 400 - clientWidth;
            	_horzScrollBarLayoutHeight = 400 - clientHeight;

                if(_vertScrollBarLayoutWidth<0)
                    _vertScrollBarLayoutWidth = 0;
                if(_horzScrollBarLayoutHeight<0)
                    _horzScrollBarLayoutHeight = 0;

            	// set the scrolled area size back to what it was
            	setDomScrolledAreaSize( _scrolledAreaSize );

                // same for the scrollview size

                styleObj.set("width", oldWidthStyle);
                styleObj.set("height", oldHeightStyle);
            }
            else
            {
            	_vertScrollBarLayoutWidth = 0;
            	_horzScrollBarLayoutHeight = 0;
            }
        }

        vertBarWidth = _vertScrollBarLayoutWidth;
        horzBarHeight = _horzScrollBarLayoutHeight;
	}


    void _addChildElement(emscripten::val childDomObject) override
    {
        emscripten::val docVal = emscripten::val::global("document");

        docVal.call<emscripten::val>("getElementById", _scrolledAreaElementId.asUtf8() ).call<void>("appendChild", childDomObject);
    }

private:

	void setDomScrolledAreaSize(const Size& size) const
	{
		emscripten::val scrolledAreaStyleObj = _scrolledAreaDomObject["style"];
    	scrolledAreaStyleObj.set("width", std::to_string((int)std::lround(size.width))+"px");
    	scrolledAreaStyleObj.set("height", std::to_string((int)std::lround(size.height))+"px");
	}

	Size                _scrolledAreaSize;

    String              _scrolledAreaElementId;
    emscripten::val     _scrolledAreaDomObject;

    mutable double      _vertScrollBarLayoutWidth = -1;
    mutable double      _horzScrollBarLayoutHeight = -1;               
};

}
}

#endif
