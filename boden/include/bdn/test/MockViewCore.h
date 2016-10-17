#ifndef BDN_TEST_MockViewCore_H_
#define BDN_TEST_MockViewCore_H_

#include <bdn/IViewCore.h>
#include <bdn/PixelAligner.h>

#include <bdn/test.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" view core that does not actually show anything
    visible, but behaves otherwise like a normal view core.
    
    See MockUiProvider.
    */
class MockViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
	explicit MockViewCore(View* pView)
        : _pixelAligner(3)  // 3 physical pixels per DIP
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_outerViewWeak = pView;

		_visible = pView->visible();
		_padding = pView->padding();
        _bounds = Rect( pView->position(), pView->size() );
		_pParentViewWeak = pView->getParentView();
	}

    ~MockViewCore()
    {
        // core objects must only be released from the main thread.
        BDN_REQUIRE_IN_MAIN_THREAD();
    }

    

    /** Returns the outer view object that this core is embedded in.*/
    P<View> getOuterViewIfStillAttached()
	{
        return _outerViewWeak.toStrong();	    
	}


    /** Returns true if the fake view is currently marked as "visible".*/
    bool getVisible() const
	{
        return _visible;	    
	}

    /** Returns the number of times the view's visibility state has changed.*/
    int getVisibleChangeCount() const
	{
	    return _visibleChangeCount;
	}


    /** Returns the padding that is currently configured.*/
    Nullable<UiMargin> getPadding() const
	{
	    return _padding;
	}

    /** Returns the number of times the view's padding has changed.*/
    int getPaddingChangeCount() const
	{
	    return _paddingChangeCount;
	}



    /** Returns the current view bounds.*/
    Rect getBounds() const
	{
	    return _bounds;
	}

    /** Returns the number of times the view's bounds have changed.*/
    int getBoundsChangeCount() const
	{
	    return _boundsChangeCount;
	}



    /** Returns the view's current parent view.
        Note that the MockViewCore does not hold a reference to it, so it
        will not keep the parent view alive. You have to ensure that the parent
        still exists when you access the returned pointer.*/
    View* getParentViewWeak() const
	{
	    return _pParentViewWeak;
	}
    
    /** Returns the number of times the view's parent have changed.*/
    int getParentViewChangeCount() const
	{
	    return _parentViewChangeCount;
	}



    




	Size _getTextSize(const String& s) const
	{
		// our fake font has a size of 9.75 x 19.60 DIPs for each character.
		return Size( s.getLength()*9.75, 19.60);
	}

	void	setVisible(const bool& visible) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_visible = visible;
		_visibleChangeCount++;
	}
	

	
	void setPadding(const Nullable<UiMargin>& padding) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_padding = padding;
		_paddingChangeCount++;
	}




    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        _bounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);

        _boundsChangeCount++;

        return _bounds;
    }


    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const override
    {
        // our mock UI has 3 pixels per DIP
        return PixelAligner(3).alignRect(requestedBounds, positionRoundType, sizeRoundType);
    }

       
	double uiLengthToDips(const UiLength& uiLength) const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

        if(uiLength.unit==UiLength::none)
			return uiLength.value;
        
        else if(uiLength.unit==UiLength::dip)
			return uiLength.value;

        else if(uiLength.unit==UiLength::em)
            // one em = 23 mock DIPs;
			return uiLength.value*23;

		else if(uiLength.unit==UiLength::sem)
            // one sem = 20 mock DIPs;
			return uiLength.value*20;

		else
			throw InvalidArgumentError("Invalid UiLength unit passed to MockViewCore::uiLengthToDips: "+std::to_string((int)uiLength.unit) );
	}

    
	Margin uiMarginToDipMargin(const UiMargin& margin) const override
    {
        BDN_REQUIRE_IN_MAIN_THREAD();

        return Margin(
            uiLengthToDips(margin.top),
            uiLengthToDips(margin.right),
            uiLengthToDips(margin.bottom),
            uiLengthToDips(margin.left) );
    }





	bool tryChangeParentView(View* pNewParent) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_pParentViewWeak = pNewParent;
		_parentViewChangeCount++;
		return true;
	}

     

protected:    
	bool		_visible = false;
	int			_visibleChangeCount = 0;


	Nullable<UiMargin>	_padding;
	int			_paddingChangeCount = 0;

	Rect        _bounds;
	int			_boundsChangeCount = 0;

	View*		_pParentViewWeak = nullptr;
	int			_parentViewChangeCount = 0;
    
	WeakP<View>	 _outerViewWeak = nullptr;

    PixelAligner _pixelAligner;

};


}
}

#endif
