#include <bdn/mac/LabelCore.hh>

#import <bdn/foundationkit/AttributedString.hh>

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

        text.onChange() += [=](auto &property) { textPropertyChanged(property.get()); };

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
        if (text->index() == 0 && std::get<0>(text.get()).empty()) {
            return ViewCore::baseline(forSize);
        }
        auto x = [_nsTextView baselineDeltaForCharacterAtIndex:0];
        return static_cast<float>(x);
    }

    void LabelCore::textPropertyChanged(const Text &text)
    {
        std::visit(
            [&nsTextView = this->_nsTextView](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, String>) {
                    nsTextView.string = fk::stringToNSString(arg);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto fkAttrString = std::dynamic_pointer_cast<bdn::fk::AttributedString>(arg)) {
                        [[nsTextView textStorage] setAttributedString:fkAttrString->nsAttributedString()];
                    }
                }
            },
            text);

        scheduleLayout();
        markDirty();
    }
}
