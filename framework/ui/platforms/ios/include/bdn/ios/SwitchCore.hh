#pragma once

#include <bdn/ui/ClickEvent.h>
#include <bdn/ui/Switch.h>

#import <bdn/ios/ViewCore.hh>

#import <UIKit/UIKit.h>

namespace bdn::ui::ios
{
    class SwitchCore;
}

@interface BdnIosSwitchComposite : UIControl <UIViewWithFrameNotification>
@property(strong) UISwitch *uiSwitch;
@property(strong) UILabel *uiLabel;
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
@end

@interface BdnIosSwitchClickManager : NSObject
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::SwitchCore> core;
@end

namespace bdn::ui::ios
{
    class SwitchCore : public ViewCore, virtual public Switch::Core
    {
      private:
        static BdnIosSwitchComposite *createSwitchComposite();

      public:
        static std::shared_ptr<SwitchCore> create();

        SwitchCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~SwitchCore();

        void init() override;

        void handleClick();

      private:
        BdnIosSwitchComposite *_composite;
        BdnIosSwitchClickManager *_clickManager;
    };
}
