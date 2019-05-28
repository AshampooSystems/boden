#pragma once

#include <bdn/log.h>
#include <bdn/ui/ClickEvent.h>
#include <bdn/ui/Label.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ViewCore.hh>

@interface BodenUILabel : UILabel <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
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

namespace bdn::ui::ios
{
    class LabelCore : public ViewCore, virtual public Label::Core
    {
      private:
        static BodenUILabel *createUILabel();

      public:
        LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        float calculateBaseline(Size forSize) const override;

      private:
        UILabel *getUILabel();

      protected:
        bool canAdjustToAvailableWidth() const override { return wrap.get(); }

      private:
        UILabel *_uiLabel;
    };
}
