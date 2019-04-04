#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/Label.h>
#include <bdn/log.h>

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
    class LabelCore : public ViewCore, virtual public bdn::Label::Core
    {
      private:
        static BodenUILabel *createUILabel();

      public:
        LabelCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      private:
        UILabel *getUILabel();

      protected:
        bool canAdjustToAvailableWidth() const override { return wrap.get(); }

      private:
        UILabel *_uiLabel;
    };
}
