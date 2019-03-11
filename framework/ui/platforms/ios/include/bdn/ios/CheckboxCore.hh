#pragma once

#include <bdn/Checkbox.h>
#include <bdn/CheckboxCore.h>
#include <bdn/ClickEvent.h>

#import <bdn/ios/IosCheckbox.hh>
#import <bdn/ios/ViewCore.hh>

@interface BdnIosCheckboxComposite : UIControl <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@property(strong) BdnIosCheckbox *checkbox;
@property(strong) UILabel *uiLabel;
@end

@class BdnIosCheckboxClickManager;

namespace bdn::ios
{
    class CheckboxCore : public ViewCore, virtual public bdn::CheckboxCore
    {
      public:
        CheckboxCore();
        virtual ~CheckboxCore();

        virtual void init() override;

        void handleClick();

      private:
        static BdnIosCheckboxComposite *_createCheckboxComposite();

      private:
        BdnIosCheckboxComposite *_composite;
        BdnIosCheckboxClickManager *_clickManager;
    };
}
