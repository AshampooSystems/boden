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
				pOuter->text().get(),
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

    P<TextView> pTextView = cast<TextView>( getOuterViewIfStillAttached() );
	String text;
    Size   maxSize;
    Size   minSize;
    if(pTextView!=nullptr)
    {
        maxSize = pTextView->preferredSizeMaximum();
        minSize = pTextView->preferredSizeMinimum();
        text = pTextView->text();
    }
    else
    {
        maxSize = Size::none();
        minSize = Size::none();
    }

    maxSize.applyMaximum(availableSpace);
        
    Size prefSize;

    {
        WindowDeviceContext dc( getHwnd() );

        double wrapWidth = maxSize.width;

        // if we are going to be bigger than the available space because of a
        // minimum size constraint then we also want to wrap according to that
        // bigger size.
        if(std::isfinite(minSize.width) && std::isfinite(wrapWidth) && minSize.width>wrapWidth )
            wrapWidth = minSize.width;

        P<const Font> pFont = getFont();
        if(pFont!=nullptr)
		    dc.setFont( *pFont );
        prefSize = dc.getTextSize( text, wrapWidth );
    }

    Nullable<UiMargin>  pad;
    if(pTextView!=nullptr)
        pad = pTextView->padding();
    UiMargin            uiPadding;
    if(pad.isNull())
    {
        // we should use the "default" padding. This is zero.
        uiPadding = UiMargin( 0 );
    }
    else
        uiPadding = pad;

	prefSize += uiMarginToDipMargin( uiPadding );	

    if(pTextView!=nullptr)
    {
        prefSize.applyMinimum(minSize);
        // clip against the max size again. It may have been exceeded if the
        // content did not fit, and we never want that.
        prefSize.applyMaximum( pTextView->preferredSizeMaximum() );
    }
    
	return prefSize;
}


}
}


