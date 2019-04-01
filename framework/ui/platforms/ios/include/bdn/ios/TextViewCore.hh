#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/TextView.h>

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
    class TextViewCore : public ViewCore, virtual public bdn::TextView::Core
    {
      private:
        static BodenUILabel *createUILabel();

      public:
        TextViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      private:
        UILabel *getUILabel();

      protected:
        bool canAdjustToAvailableWidth() const override { return true; }

      private:
        UILabel *_uiLabel;
    };
}
