#ifndef BDN_TEST_MockTextViewCore_H_
#define BDN_TEST_MockTextViewCore_H_


#include <bdn/test/MockViewCore.h>

#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" text view core that does not actually show anything
    visible, but behaves otherwise like a normal text view core.
    
    See MockUiProvider.
    */
class MockTextViewCore : public MockViewCore, BDN_IMPLEMENTS ITextViewCore
{
public:
	MockTextViewCore(TextView* pView)
		: MockViewCore(pView)
	{
		_text = pView->text();		
	}


    /** Returns the current text content of the text view.*/
    String getText() const
	{
	    return _text;
	}

    /** Returns the number of times that a new text has been set.*/
    int getTextChangeCount() const
	{
	    return _textChangeCount;
    }
	
	void setText(const String& text) override
	{
		_text = text;
		_textChangeCount++;
	}


	
    Size calcPreferredSize( const Size& minSize = Size::none(),
                            const Size& maxSize = Size::none() ) const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		Size size = _getTextSize(_text);

        // add our padding
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr && !pView->padding().get().isNull())
            size += uiMarginToDipMargin(pView->padding().get());

        size.applyConstraints(minSize, maxSize);
        
		return size;
	}
	

protected:    
	String _text;
	int    _textChangeCount = 0;
};

}
}

#endif
