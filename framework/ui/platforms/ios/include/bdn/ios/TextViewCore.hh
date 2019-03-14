#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/TextView.h>
#include <bdn/TextViewCore.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ViewCore.hh>

@interface BodenUILabel : UILabel <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@implementation BodenUILabel
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}
@end

namespace bdn::ios
{
    class TextViewCore : public ViewCore, virtual public bdn::TextViewCore
    {
      private:
        static BodenUILabel *createUILabel()
        {
            BodenUILabel *label = [[BodenUILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
            label.numberOfLines = 0;
            return label;
        }

      public:
        TextViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider) : ViewCore(uiProvider, createUILabel())
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

        UILabel *getUILabel() { return _uiLabel; }

      protected:
        double getFontSize() const override { return _uiLabel.font.pointSize; }

        bool canAdjustToAvailableWidth() const override { return true; }

      private:
        UILabel *_uiLabel;
    };
}
