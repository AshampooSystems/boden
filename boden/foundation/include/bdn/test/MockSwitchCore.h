#ifndef BDN_TEST_MockSwitchCore_H_
#define BDN_TEST_MockSwitchCore_H_


#include <bdn/test/MockViewCore.h>

#include <bdn/ISwitchCore.h>
#include <bdn/Switch.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" Switch core that does not actually show anything
    visible, but behaves otherwise like a normal Switch core.
    
    See MockUiProvider.
    */
class MockSwitchCore : public MockViewCore, BDN_IMPLEMENTS ISwitchCore
{
public:
	MockSwitchCore(Switch* pSwitch)
		: MockViewCore(pSwitch)
	{
		_label = pSwitch->label();
        _on = pSwitch->on();
	}


    /** Returns the current label of the Switch.*/
    String getLabel() const
	{
	    return _label;
	}

    /** Returns the number of times that a new label has been set.*/
    int getLabelChangeCount() const
	{
	    return _labelChangeCount;
    }
	
	void setLabel(const String& label) override
	{
		_label = label;
		_labelChangeCount++;
	}
    
    bool getOn() const
    {
        return _on;
    }
    
    int getOnChangeCount() const
    {
        return _onChangeCount;
    }
    
    void setOn(const bool& on) override
    {
        _on = on;
        _onChangeCount++;
    }
    
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        MockViewCore::calcPreferredSize(availableSpace);

		BDN_REQUIRE_IN_MAIN_THREAD();

		Size size = _getTextSize(_label);

        

        // add our padding
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            if(!pView->padding().isNull())
                size += uiMarginToDipMargin(pView->padding());
        }

        // add some space for the fake Switch border
		size += Margin( 4, 5);

        // ignore available space. We have a fixed size.
        
        if(pView!=nullptr)
        {            
            // clip to min and max size
            size.applyMinimum( pView->preferredSizeMinimum() );
            size.applyMaximum( pView->preferredSizeMaximum() );
        }

		return size;
	}
	
	
	void generateClick()
	{
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
		    ClickEvent evt(pView);

		    cast<Switch>(pView)->onClick().notify(evt);
        }
	}

protected:    
	String _label;
    int    _labelChangeCount = 0;
    bool   _on;
    int    _onChangeCount = 0;
};

}
}

#endif // BDN_TEST_MockSwitchCore_H_
