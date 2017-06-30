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
		BDN_REQUIRE_IN_MAIN_THREAD();

		Size size = _getTextSize(_text);

        // add our padding
        P<View> pView = getOuterViewIfStillAttached();
        Size    preferredSizeHint(Size::none());
        if(pView!=nullptr)
        {
            if(!pView->padding().get().isNull())
                size += uiMarginToDipMargin(pView->padding().get());

            preferredSizeHint = pView->preferredSizeHint();
        }

        // text views typically somewhat adhere to the available width.
        // We do not do real line breaking in this mock view - we simply clip the width
        // and multiply the height with a corresponding factor.
        if( std::isfinite(availableSpace.width) && size.width>availableSpace.width)
        {
            double factor = (availableSpace.width<=1) ? 100 : (size.width/availableSpace.width);
            if(factor>100)
                factor = 100;

            size.width = availableSpace.width;
            size.height *= factor;
        }

        // we also clip to the preferredSizeHint.width, since text views usually use
        // that as an advisory value of where to clip
        size.applyMaximum( Size(preferredSizeHint.width, Size::componentNone()) );

        if(pView!=nullptr)
        {   
            // clip to min and max size
            size.applyMinimum( pView->preferredSizeMinimum() );
            size.applyMaximum( pView->preferredSizeMaximum() );
        }

        
		return size;
	}

	void layout() override
	{
		// nothing to do
	}
	

protected:    
	String _text;
	int    _textChangeCount = 0;
};

}
}

#endif
