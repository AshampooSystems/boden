#import <bdn/foundationkit/AttributedString.hh>
#include <bdn/ios/LabelCore.hh>

namespace bdn::ui::detail
{
    CORE_REGISTER(Label, bdn::ui::ios::LabelCore, Label)
}

namespace bdn::ui::ios
{
    BodenUILabel *LabelCore::createUILabel()
    {
        BodenUILabel *label = [[BodenUILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        label.numberOfLines = 0;

        return label;
    }

    LabelCore::LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createUILabel())
    {
        _uiLabel = (UILabel *)uiView();

        text.onChange() += [=](auto &property) { textChanged(property.get()); };

        wrap.onChange() += [=](auto &property) {
            _uiLabel.numberOfLines = wrap ? 0 : 1;
            markDirty();
        };
    }

    UILabel *LabelCore::getUILabel() { return _uiLabel; }

    float LabelCore::calculateBaseline(bdn::Size forSize) const
    {
        auto s = _uiLabel.intrinsicContentSize.height;
        auto offset = std::max(((forSize.height) / 2.0) - (s / 2.0), 0.0);

        auto ascender = round(_uiLabel.font.ascender);

        return static_cast<float>(ascender + offset);
    }

    void LabelCore::textChanged(const Text &text)
    {
        std::visit(
            [uiLabel = this->_uiLabel](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, String>) {
                    uiLabel.text = fk::stringToNSString(arg);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto fkAttrString = std::dynamic_pointer_cast<bdn::fk::AttributedString>(arg)) {
                        uiLabel.attributedText = fkAttrString->nsAttributedString();
                    }
                }
            },
            text);

        markDirty();
    }
}
