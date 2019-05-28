#include <bdn/mac/LabelCore.hh>

namespace bdn::ui::detail
{
    CORE_REGISTER(Label, bdn::ui::mac::LabelCore, Label)
}

namespace bdn::ui::mac
{
    NSTextView *createNSTextView()
    {
        NSTextView *view = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

        view.editable = static_cast<BOOL>(false);
        view.selectable = static_cast<BOOL>(false);
        view.richText = static_cast<BOOL>(false);

        view.verticallyResizable = static_cast<BOOL>(false);
        view.textContainer.lineFragmentPadding = 0.0;

        // do not draw the background by default
        view.drawsBackground = NO;

        return view;
    }

    LabelCore::LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : mac::ViewCore(viewCoreFactory, createNSTextView())
    {
        _nsTextView = (NSTextView *)nsView();

        text.onChange() += [=](auto &property) {
            NSString *macText = fk::stringToNSString(property.get());
            _nsTextView.string = macText;

            [_nsTextView.layoutManager glyphRangeForTextContainer:_nsTextView.textContainer];
            scheduleLayout();
            markDirty();
        };

        wrap.onChange() += [=](auto &property) {
            _wrap = wrap;
            scheduleLayout();
            markDirty();
        };
    }

    Size LabelCore::sizeForSpace(Size availableSpace) const
    {
        CGSize boundingRect = CGSizeMake(_wrap ? availableSpace.width : CGFLOAT_MAX, CGFLOAT_MAX);

        CGRect r = [[_nsTextView textStorage]
            boundingRectWithSize:boundingRect
                         options:NSStringDrawingUsesLineFragmentOrigin | NSStringDrawingUsesFontLeading
                         context:nil];

        Size insetSize = macSizeToSize(_nsTextView.textContainerInset);
        insetSize += insetSize;

        Size result = macSizeToSize(r.size) + insetSize;

        return result;
    }

    float LabelCore::calculateBaseline(Size forSize, bool forIndicator) const
    {
        if (text->empty()) {
            return ViewCore::baseline(forSize);
        }
        auto x = [_nsTextView baselineDeltaForCharacterAtIndex:0];
        return static_cast<float>(x);
    }
}
