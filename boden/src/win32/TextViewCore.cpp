#include <bdn/init.h>
#include <bdn/win32/TextViewCore.h>

#include <bdn/win32/WindowDeviceContext.h>

namespace bdn
{
namespace win32
{

TextViewCore::TextViewCore(TextView* pOuter)
	: ViewCore(	pOuter, 
				"STATIC",
				pOuter->text(),
				SS_NOPREFIX | WS_CHILD,
				0 )
{
}

void TextViewCore::setText(const String& text)
{
	setWindowText(getHwnd(), text);
}

Size TextViewCore::calcPreferredSize( const Size& availableSpace ) const
{
	// note that we ignore availableHeight because there is no way for us to reduce
	// the height of the text. We can only limit the width (by wrapping lines)

    P<TextView>          pTextView = cast<TextView>( getOuterViewIfStillAttached() );
	String               text;
    Size                 maxSize;
    Size                 minSize;
    Size                 hintSize;
    Nullable<UiMargin>   pad;

    if(pTextView!=nullptr)
    {
        maxSize = pTextView->preferredSizeMaximum();
        minSize = pTextView->preferredSizeMinimum();
		hintSize = pTextView->preferredSizeHint();
        text = pTextView->text();
        pad = pTextView->padding();
    }
    else
    {
        maxSize = Size::none();
        minSize = Size::none();
        hintSize = Size::none();
    }

    UiMargin            uiPadding;
    if(pad.isNull())
    {
        // we should use the "default" padding. This is zero.
        uiPadding = UiMargin( 0 );
    }
    else
        uiPadding = pad;

    Margin dipPadding = uiMarginToDipMargin( uiPadding );

    Size prefSize;

    {
        WindowDeviceContext dc( getHwnd() );

        Size wrapSize(hintSize);

        wrapSize.applyMaximum(availableSpace);
        
        // if we have a maximum and it is less than the hint then we wrap according
        // to that.
        wrapSize.applyMaximum(maxSize);

        double wrapWidth = wrapSize.width;
        if( std::isfinite(wrapWidth))
            wrapWidth -= dipPadding.left + dipPadding.right;

        P<const Font> pFont = getFont();
        if(pFont!=nullptr)
		    dc.setFont( *pFont );
        prefSize = dc.getTextSize( text, wrapWidth );
    }

    
	prefSize += dipPadding;

    if(pTextView!=nullptr)
    {
        prefSize.applyMinimum(minSize);
        // clip against the max size again. It may have been exceeded if the
        // content did not fit, and we never want that.
        prefSize.applyMaximum( maxSize );
    }
    
	return prefSize;
}


}
}


