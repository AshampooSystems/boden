#pragma once

#include <bdn/ui/Checkbox.h>
#include <bdn/ui/ClickEvent.h>

#import <bdn/ios/IosCheckbox.hh>
#import <bdn/ios/ViewCore.hh>

@interface BdnIosCheckboxComposite : UIControl <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
@property(strong) BdnIosCheckbox *checkbox;
@property(strong) UILabel *uiLabel;
@end

@class BdnIosCheckboxClickManager;

namespace bdn::ui::ios
{
    class CheckboxCore : public ViewCore, virtual public Checkbox::Core
    {
      public:
        CheckboxCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~CheckboxCore();

        void init() override;

        void handleClick();

      private:
        static BdnIosCheckboxComposite *_createCheckboxComposite();

      private:
        BdnIosCheckboxComposite *_composite;
        BdnIosCheckboxClickManager *_clickManager;
    };
}
