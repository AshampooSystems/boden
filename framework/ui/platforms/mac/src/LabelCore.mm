#include <bdn/mac/LabelCore.hh>

namespace bdn::detail
{
    CORE_REGISTER(Label, bdn::mac::LabelCore, Label)
}

namespace bdn::mac
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

    LabelCore::LabelCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : bdn::mac::ViewCore(viewCoreFactory, createNSTextView())
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
}
