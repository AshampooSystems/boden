#import <Cocoa/Cocoa.h>
#import <bdn/mac/ButtonCoreBase.hh>

namespace bdn::mac
{
    ButtonCoreBase::ButtonCoreBase(std::shared_ptr<View> outer, NSButton *nsButton) : ChildViewCore(outer, nsButton)
    {
        _nsButton = nsButton;
    }

    void ButtonCoreBase::setLabel(const String &label)
    {
        NSString *macLabel = stringToNSString(label);
        [_nsButton setTitle:macLabel];
    }

    double ButtonCoreBase::getFontSize() const { return _nsButton.font.pointSize; }
}
