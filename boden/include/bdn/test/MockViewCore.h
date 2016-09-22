#ifndef BDN_TEST_MockViewCore_H_
#define BDN_TEST_MockViewCore_H_

#include <bdn/IViewCore.h>

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
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_outerViewWeak = pView;

		_visible = pView->visible();
		_padding = pView->padding();
        _position = pView->position();
		_size = pView->size();
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



    /** Returns the current view position.*/
    Point getPosition() const
	{
	    return _position;
	}

    /** Returns the number of times the view's position has changed.*/
    int getPositionChangeCount() const
	{
	    return _positionChangeCount;
	}


    /** Returns the current view size.*/
    Size getSize() const
	{
	    return _size;
	}

    /** Returns the number of times the view's size has changed.*/
    int getSizeChangeCount() const
	{
	    return _sizeChangeCount;
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
		// our fake font has a size of 10x20 DIPs for each character.
		return Size( static_cast<int>( s.getLength()*10 ), 20);
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

	void setPosition(const Point& position) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_position = position;
		_positionChangeCount++;
	}


    void setSize(const Size& size) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_size = size;
		_sizeChangeCount++;
	}

       
	double uiLengthToDips(const UiLength& uiLength) const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		if(uiLength.unit==UiLength::Unit::sem)
		{
			// one sem = 20 mock DIPs;
			return uiLength.value*20;
		}
		else if(uiLength.unit==UiLength::Unit::dip)
		{
			return uiLength.value;
		}
		else
		{
			// invalid parameter passed to this function
			REQUIRE(false);
			return 0;
		}
	}
	

	Margin uiMarginToDipMargin(const UiMargin& margin) const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		return Margin( uiLengthToDips(margin.top),
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

	Point       _position;
	int			_positionChangeCount = 0;

    Size        _size;
	int			_sizeChangeCount = 0;

	View*		_pParentViewWeak = nullptr;
	int			_parentViewChangeCount = 0;
    
	WeakP<View>	 _outerViewWeak = nullptr;

};


}
}

#endif
