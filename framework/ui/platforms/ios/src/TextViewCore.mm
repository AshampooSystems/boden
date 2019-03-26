#include <bdn/ios/TextViewCore.hh>

namespace bdn::detail
{
    CORE_REGISTER(TextView, bdn::ios::TextViewCore, TextView)
}

namespace bdn::ios
{
    BodenUILabel *TextViewCore::createUILabel()
    {
        BodenUILabel *label = [[BodenUILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        label.numberOfLines = 0;
        return label;
    }

    TextViewCore::TextViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createUILabel())
    {
        _uiLabel = (UILabel *)uiView();

        text.onChange() += [=](auto va) {
            _uiLabel.text = fk::stringToNSString(text);
            markDirty();
        };

        wrap.onChange() += [=](auto va) {
            _uiLabel.numberOfLines = wrap ? 0 : 1;
            markDirty();
        };
    }

    UILabel *TextViewCore::getUILabel() { return _uiLabel; }
}
