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
                // So we use the clientWidth and clientHeight properties instead. Those are rounded integer values,
                // but that is ok for these purposes - it does not matter that much if we allocate too much layout space
                // for the scroll bar. It would just increase the spacing between scroll bar and content slightly.

                double clientWidth = _domObject["clientWidth"].as<double>();
                double clientHeight = _domObject["clientHeight"].as<double>();

            	_vertScrollBarLayoutWidth = 400 - clientWidth;
            	_horzScrollBarLayoutHeight = 400 - clientHeight;

                if(_vertScrollBarLayoutWidth<0)
                    _vertScrollBarLayoutWidth = 0;
                if(_horzScrollBarLayoutHeight<0)
                    _horzScrollBarLayoutHeight = 0;

                // we add +1 to the sizes, in case clientWidth and clientHeight were rounded up.
                // It is ok if we allocate 1 pixel too much, but inacceptable if we allocate 1 pixel too little.
                _vertScrollBarLayoutWidth++;
                _horzScrollBarLayoutHeight++;            	

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
