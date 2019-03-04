#pragma once

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
    NSPoint displacedOrigin =
        NSMakePoint(origin.x + _textContainerDisplacement.width, origin.y + _textContainerDisplacement.height);

    return displacedOrigin;
}

@end

namespace bdn::mac
{
    class TextViewCore : public ChildViewCore, virtual public ITextViewCore
    {
      private:
        static BdnMacTextView_ *_createNSTextView(std::shared_ptr<TextView> outerTextView)
        {
            BdnMacTextView_ *view = [[BdnMacTextView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

            view.editable = false;
            view.selectable = false;
            view.richText = false;

            view.verticallyResizable = false;

            // do not draw the background by default
            view.drawsBackground = NO;

            return view;
        }

      public:
        TextViewCore(std::shared_ptr<TextView> outerTextView)
            : ChildViewCore(outerTextView, _createNSTextView(outerTextView))
        {
            _nsTextView = (BdnMacTextView_ *)nsView();

            setText(outerTextView->text);
            _wrap = outerTextView->wrap;
        }

        void setText(const String &text) override
        {
            NSString *macText = stringToNSString(text);
            _nsTextView.string = macText;

            // force immediate re-layout
            [_nsTextView.layoutManager glyphRangeForTextContainer:_nsTextView.textContainer];
        }

        void setWrap(const bool &wrap) override { _wrap = wrap; }

        Size sizeForSpace(Size availableSpace) const override
        {
            CGSize boundingRect = CGSizeMake(_wrap ? availableSpace.width : CGFLOAT_MAX, CGFLOAT_MAX);

            CGRect r = [[_nsTextView textStorage]
                boundingRectWithSize:boundingRect
                             options:NSStringDrawingUsesLineFragmentOrigin | NSStringDrawingUsesFontLeading
                             context:nil];

            Size insetSize = macSizeToSize(_nsTextView.textContainerInset);
            insetSize.applyMinimum({0, 0});
            insetSize += insetSize;

            Size result = macSizeToSize(r.size) + insetSize;

            // Its magic !
            result.width += 10;

            return result;
        }

      protected:
        double getFontSize() const override { return _nsTextView.font.pointSize; }

      private:
        BdnMacTextView_ *_nsTextView;
        bool _wrap;
    };
}
