#pragma once

#include <bdn/TextView.h>
#include <bdn/TextViewCore.h>

#import <bdn/mac/ViewCore.hh>
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
    class TextViewCore : public ViewCore, virtual public bdn::TextViewCore
    {
      private:
        static BdnMacTextView_ *_createNSTextView()
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
        TextViewCore() : ViewCore(_createNSTextView())
        {
            _nsTextView = (BdnMacTextView_ *)nsView();

            text.onChange() += [=](auto va) {
                NSString *macText = stringToNSString(va->get());
                _nsTextView.string = macText;

                [_nsTextView.layoutManager glyphRangeForTextContainer:_nsTextView.textContainer];
                scheduleLayout();
            };

            wrap.onChange() += [=](auto va) {
                _wrap = wrap;
                scheduleLayout();
            };
        }

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
