#ifndef BDN_TEST_MockCheckboxCore_H_
#define BDN_TEST_MockCheckboxCore_H_


#include <bdn/test/MockViewCore.h>

#include <bdn/ICheckboxCore.h>
#include <bdn/Checkbox.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" checkbox core that does not actually show anything
    visible, but behaves otherwise like a normal checkbox core.
    
    See MockUiProvider.
    */
class MockCheckboxCore : public MockViewCore, BDN_IMPLEMENTS ICheckboxCore
{
public:
	MockCheckboxCore(Checkbox* pCheckbox)
		: MockViewCore(pCheckbox)
	{
		_label = pCheckbox->label();
        _state = pCheckbox->state();
	}


    /** Returns the current label of the Checkbox.*/
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
    
    TriState getState() const
    {
        return _state;
    }
    
    int getStateChangeCount() const
    {
        return _stateChangeCount;
    }
    
    void setState(const TriState &state) override
    {
        _state = state;
        _stateChangeCount++;
    }

	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        MockViewCore::calcPreferredSize(availableSpace);

		BDN_REQUIRE_IN_MAIN_THREAD();

		Size size = _getTextSize( _label );

        

        // add our padding
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            if(!pView->padding().isNull())
                size += uiMarginToDipMargin(pView->padding());
        }

        // add some space for the fake Checkbox border
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

		    cast<Checkbox>(pView)->onClick().notify(evt);
        }
	}

protected:    
	String   _label;
    int      _labelChangeCount = 0;
    TriState _state;
    int      _stateChangeCount = 0;
};

}
}

#endif // BDN_TEST_MockCheckboxCore_H_
