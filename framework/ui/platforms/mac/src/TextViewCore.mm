#include <bdn/mac/TextViewCore.hh>

namespace bdn::detail
{
    CORE_REGISTER(TextView, bdn::mac::TextViewCore, TextView)
}

namespace bdn::mac
{
    BdnMacTextView_ *TextViewCore::_createNSTextView()
    {
        BdnMacTextView_ *view = [[BdnMacTextView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

        view.editable = static_cast<BOOL>(false);
        view.selectable = static_cast<BOOL>(false);
        view.richText = static_cast<BOOL>(false);

        view.verticallyResizable = static_cast<BOOL>(false);

        // do not draw the background by default
        view.drawsBackground = NO;

        return view;
    }

    TextViewCore::TextViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : bdn::mac::ViewCore(viewCoreFactory, _createNSTextView())
    {
        _nsTextView = (BdnMacTextView_ *)nsView();

        text.onChange() += [=](auto va) {
            NSString *macText = fk::stringToNSString(va->get());
            _nsTextView.string = macText;

            [_nsTextView.layoutManager glyphRangeForTextContainer:_nsTextView.textContainer];
            scheduleLayout();
            markDirty();
        };

        wrap.onChange() += [=](auto va) {
            _wrap = wrap;
            scheduleLayout();
            markDirty();
        };
    }

    Size TextViewCore::sizeForSpace(Size availableSpace) const
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
}
