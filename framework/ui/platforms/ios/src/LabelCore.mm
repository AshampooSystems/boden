#include <bdn/ios/LabelCore.hh>

namespace bdn::detail
{
    CORE_REGISTER(Label, bdn::ios::LabelCore, Label)
}

namespace bdn::ios
{
    BodenUILabel *LabelCore::createUILabel()
    {
        BodenUILabel *label = [[BodenUILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        label.numberOfLines = 0;
        return label;
    }

    LabelCore::LabelCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
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
}
