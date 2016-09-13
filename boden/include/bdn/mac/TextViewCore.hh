#ifndef BDN_MAC_TextViewCore_HH_
#define BDN_MAC_TextViewCore_HH_

#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>

#import <bdn/mac/ChildViewCore.hh>
#import <bdn/mac/util.hh>


/** NSTextView subclass that adds some additional capabilities.

    For example, non-symmetrical text-insets can be set.
 */
@interface BdnMacTextView_ : NSTextView

    /** A displacement value that is added to textContainerOrigin.
        This can be used to create non-symmetrical insets / padding.*/
    @property NSSize textContainerDisplacement;

@end


@implementation BdnMacTextView_


- (NSPoint)textContainerOrigin
{
    NSPoint origin = [super textContainerOrigin];
    NSPoint displacedOrigin = NSMakePoint(origin.x + _textContainerDisplacement.width,
                                          origin.y + _textContainerDisplacement.height);
    
    return displacedOrigin;
}

@end


namespace bdn
{
namespace mac
{

class TextViewCore : public ChildViewCore, BDN_IMPLEMENTS ITextViewCore
{
private:
    static BdnMacTextView_* _createNSTextView(TextView* pOuterTextView)
    {
        BdnMacTextView_* view = [[BdnMacTextView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        
        view.editable = false;
        view.selectable = false;
        view.richText = false;
        
        // do not draw the background by default
        view.drawsBackground = NO;
        
        return view;
    }
    

public:
    TextViewCore(TextView* pOuterTextView)
    : ChildViewCore( pOuterTextView, _createNSTextView(pOuterTextView) )
    {
        _nsTextView = (BdnMacTextView_*)getNSView();
        
        setText( pOuterTextView->text() );
    }
        
    void setText(const String& text) override
    {
        NSString* macText = stringToMacString(text);
        _nsTextView.string = macText;
        
        // force immediate re-layout
        [_nsTextView.layoutManager glyphRangeForTextContainer:_nsTextView.textContainer];
    }
    
    
    void setPadding(const Nullable<UiMargin>& padding) override
    {
        // we can set an "inset" for the text.
        // However, that is CGSize value that is used as both the
        // left/top and the right/bottom margin. In other words, the text will
        // always be centered.
        // Because of this we have subclassed NSTextView to add a custom
        // property called textContainerOriginDisplacement. It can be used
        // to move the text container from its middle position to one side
        // or the other.
        
        Margin pixelPadding;
        if(!padding.isNull())
            pixelPadding = uiMarginToPixelMargin(padding.get());
        
        int paddingWidth = pixelPadding.left + pixelPadding.right;
        int paddingHeight = pixelPadding.top + pixelPadding.bottom;
        
        // by default the text container
        NSSize inset;
        inset.width = paddingWidth/2;
        inset.height = paddingHeight/2;
        
        NSSize displacement;
        displacement.width = pixelPadding.left - inset.width;
        displacement.height = pixelPadding.top - inset.height;
        
        
        _nsTextView.textContainerInset = inset;
        _nsTextView.textContainerDisplacement = displacement;
        
        [_nsTextView invalidateTextContainerOrigin];
    }
    
    void setBounds(const Rect& bounds) override
    {
        // by default the text view will automatically adjust its height
        // to match the content size. We want it to have exactly the desired size,
        // so we explicitly set a constraint.
        _nsTextView.minSize = sizeToMacSize( bounds.getSize() );
    
        ChildViewCore::setBounds(bounds);
    }

    
    
    
    Size calcPreferredSize() const override
    {
        NSTextStorage*      textStorage = [[NSTextStorage alloc] initWithString:_nsTextView.string ];
        NSTextContainer*    textContainer = [[NSTextContainer alloc]
                                           initWithContainerSize: NSMakeSize(FLT_MAX, FLT_MAX)];
        NSLayoutManager*    layoutManager = [[NSLayoutManager alloc] init];
        
        [layoutManager addTextContainer:textContainer];
        [textStorage addLayoutManager:layoutManager];
        
        [textStorage addAttribute:NSFontAttributeName value:_nsTextView.font
                            range:NSMakeRange(0, [textStorage length])];
        [textContainer setLineFragmentPadding:0];
        
        // force immediate layout
        (void) [layoutManager glyphRangeForTextContainer:textContainer];
        
        NSSize macSize = [layoutManager
                usedRectForTextContainer:textContainer].size;

        Size size = macSizeToSize( macSize );
        
        if(size.width<0)
            size.width = 0;
        if(size.height<0)
            size.height = 0;

        Size insetSize = macSizeToSize( _nsTextView.textContainerInset );
        if(insetSize.width<0)
            insetSize.width = 0;
        if(insetSize.height<0)
            insetSize.height = 0;
        
        // add the inset size twice (once for top/left and once for bottom/right)
        size += insetSize;
        size += insetSize;
        
        NSRect boundingRect = [_nsTextView.layoutManager boundingRectForGlyphRange:NSMakeRange(0, [textStorage length])
                                                        inTextContainer:_nsTextView.textContainer ];
        
        // add margins
        size.width += std::ceil(boundingRect.origin.x) * 2;
        size.height += std::ceil(boundingRect.origin.y) * 2;
        
        return size;
    }
    

    
    
private:
    BdnMacTextView_*   _nsTextView;
};

}
}


#endif
