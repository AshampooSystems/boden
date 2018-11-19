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
    NSPoint displacedOrigin =
        NSMakePoint(origin.x + _textContainerDisplacement.width, origin.y + _textContainerDisplacement.height);

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
            static BdnMacTextView_ *_createNSTextView(TextView *outerTextView)
            {
                BdnMacTextView_ *view = [[BdnMacTextView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

                view.editable = false;
                view.selectable = false;
                view.richText = false;

                // do not draw the background by default
                view.drawsBackground = NO;

                return view;
            }

          public:
            TextViewCore(TextView *outerTextView) : ChildViewCore(outerTextView, _createNSTextView(outerTextView))
            {
                _nsTextView = (BdnMacTextView_ *)getNSView();

                setText(outerTextView->text());
            }

            void setText(const String &text) override
            {
                NSString *macText = stringToMacString(text);
                _nsTextView.string = macText;

                // force immediate re-layout
                [_nsTextView.layoutManager glyphRangeForTextContainer:_nsTextView.textContainer];
            }

            void setPadding(const Nullable<UiMargin> &padding) override
            {
                // we can set an "inset" for the text.
                // However, that is CGSize value that is used as both the
                // left/top and the right/bottom margin. In other words, the
                // text will always be centered. Because of this we have
                // subclassed NSTextView to add a custom property called
                // textContainerOriginDisplacement. It can be used to move the
                // text container from its middle position to one side or the
                // other.

                Margin dipPadding;
                if (!padding.isNull())
                    dipPadding = uiMarginToDipMargin(padding.get());

                int paddingWidth = dipPadding.left + dipPadding.right;
                int paddingHeight = dipPadding.top + dipPadding.bottom;

                // by default the text container
                NSSize inset;
                inset.width = paddingWidth / 2;
                inset.height = paddingHeight / 2;

                NSSize displacement;
                displacement.width = dipPadding.left - inset.width;
                displacement.height = dipPadding.top - inset.height;

                _nsTextView.textContainerInset = inset;
                _nsTextView.textContainerDisplacement = displacement;

                [_nsTextView invalidateTextContainerOrigin];
            }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                // by default the text view will automatically adjust its height
                // to match the content size. We want it to have exactly the
                // desired size, so we explicitly set a constraint.
                _nsTextView.minSize = sizeToMacSize(requestedBounds.getSize());

                return ChildViewCore::adjustAndSetBounds(requestedBounds);
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                NSTextStorage *textStorage = [[NSTextStorage alloc] initWithString:_nsTextView.string];
                NSTextContainer *textContainer =
                    [[NSTextContainer alloc] initWithContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
                NSLayoutManager *layoutManager = [[NSLayoutManager alloc] init];

                [layoutManager addTextContainer:textContainer];
                [textStorage addLayoutManager:layoutManager];

                [textStorage addAttribute:NSFontAttributeName
                                    value:_nsTextView.font
                                    range:NSMakeRange(0, [textStorage length])];
                [textContainer setLineFragmentPadding:0];

                Size insetSize = macSizeToSize(_nsTextView.textContainerInset);
                if (insetSize.width < 0)
                    insetSize.width = 0;
                if (insetSize.height < 0)
                    insetSize.height = 0;

                // add the inset size twice (once for top/left and once for
                // bottom/right)
                Size additionalSpace = insetSize + insetSize;

                // add margins
                NSRect boundingMacRect =
                    [_nsTextView.layoutManager boundingRectForGlyphRange:NSMakeRange(0, [textStorage length])
                                                         inTextContainer:_nsTextView.textContainer];

                Rect boundingRect = macRectToRect(boundingMacRect, -1);

                additionalSpace.width += boundingRect.x * 2;
                additionalSpace.height += boundingRect.y * 2;

                // note that we ignore availableHeight. There is nothing the
                // implementation can do to shrink in height anyway. We also use
                // the preferred size hint to define the wrapping width. Last
                // but not least we also include the max width, if we have one,
                // so that the wrapping will take that into account as well.
                Size wrapSize = availableSpace;

                P<const View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    wrapSize.applyMaximum(view->preferredSizeHint());
                    wrapSize.applyMaximum(view->preferredSizeMaximum());
                }

                if (std::isfinite(wrapSize.width))
                    textContainer.size = NSMakeSize(wrapSize.width - additionalSpace.width, textContainer.size.height);

                // force immediate layout
                (void)[layoutManager glyphRangeForTextContainer:textContainer];

                NSSize macSize = [layoutManager usedRectForTextContainer:textContainer].size;

                Size size = macSizeToSize(macSize);

                if (size.width < 0)
                    size.width = 0;
                if (size.height < 0)
                    size.height = 0;

                size += additionalSpace;

                if (view != nullptr) {
                    size.applyMinimum(view->preferredSizeMinimum());
                    size.applyMaximum(view->preferredSizeMaximum());
                }

                return size;
            }

          protected:
            double getFontSize() const override { return _nsTextView.font.pointSize; }

          private:
            BdnMacTextView_ *_nsTextView;
        };
    }
}

#endif
