#ifndef BDN_TEST_MockButtonCore_H_
#define BDN_TEST_MockButtonCore_H_


#include <bdn/test/MockViewCore.h>

#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" button core that does not actually show anything
    visible, but behaves otherwise like a normal button core.
    
    See MockUiProvider.
    */
class MockButtonCore : public MockViewCore, BDN_IMPLEMENTS IButtonCore
{
public:
	MockButtonCore(Button* pButton)
		: MockViewCore(pButton)
	{
		_label = pButton->label();		
	}


    /** Returns the current label of the button.*/
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


	
	Size calcPreferredSize() const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		Size size = _getTextSize(_label);

		// add some space for the fake button border
		size += Margin( 4, 5);

		return size;
	}
	
	
	void generateClick()
	{
		ClickEvent evt(getOuterViewWeak());

		cast<Button>(getOuterViewWeak())->onClick().notify(evt);
	}

protected:    
	String _label;
	int    _labelChangeCount = 0;
};

}
}

#endif