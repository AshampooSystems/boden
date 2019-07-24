#pragma once

#include <bdn/log.h>
#include <bdn/ui/ClickEvent.h>
#include <bdn/ui/Label.h>

#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/ios/ViewCore.hh>

namespace bdn::ui::ios
{
    class LabelCore;
}

@interface BodenUILabel : UITextView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;

- (id)linkAt:(CGPoint)pos;
@end

namespace bdn::ui::ios
{
    class LabelCore : public ViewCore, virtual public Label::Core
    {
      private:
        static BodenUILabel *createUILabel(LabelCore *core);

      public:
        LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        float calculateBaseline(Size forSize) const override;

      private:
        UITextView *getUILabel();

      protected:
        bool canAdjustToAvailableWidth() const override { return wrap.get(); }
        void textChanged(const Text &text);

      private:
        UITextView *_uiLabel;
    };
}
