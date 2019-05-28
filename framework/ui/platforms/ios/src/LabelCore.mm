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

        text.onChange() += [=](auto &property) {
            _uiLabel.text = fk::stringToNSString(text);
            markDirty();
        };

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
}
