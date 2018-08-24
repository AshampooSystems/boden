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


	
    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        MockViewCore::calcPreferredSize(availableSpace);

		BDN_REQUIRE_IN_MAIN_THREAD();

        Size    preferredSizeHint(Size::none());
        Margin  padding;

        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            if(!pView->padding().isNull())
                padding = uiMarginToDipMargin(pView->padding());

            preferredSizeHint = pView->preferredSizeHint();
        }

        double wrapWidth = preferredSizeHint.width;

        if( std::isfinite(availableSpace.width) && availableSpace.width < wrapWidth )
            wrapWidth = availableSpace.width;

        if(std::isfinite(wrapWidth))
            wrapWidth -= padding.left + padding.right;

        if(wrapWidth<0)
            wrapWidth = 0;

		Size size = _getTextSize(_text, wrapWidth);

        size += padding;
                
        if(pView!=nullptr)
        {   
            // clip to min and max size
            size.applyMinimum( pView->preferredSizeMinimum() );
            size.applyMaximum( pView->preferredSizeMaximum() );
        }

        
		return size;
	}
    	

protected:    
	String _text;
	int    _textChangeCount = 0;
};

}
}

#endif
