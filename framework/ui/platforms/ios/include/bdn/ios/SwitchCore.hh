#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/Switch.h>
#include <bdn/SwitchCore.h>

#import <bdn/ios/ViewCore.hh>

#import <UIKit/UIKit.h>

namespace bdn::ios
{
    class SwitchCore;
}

@interface BdnIosSwitchComposite : UIControl <UIViewWithFrameNotification>
@property(strong) UISwitch *uiSwitch;
@property(strong) UILabel *uiLabel;
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@interface BdnIosSwitchClickManager : NSObject
@property(nonatomic, assign) std::weak_ptr<bdn::ios::SwitchCore> core;
@end

namespace bdn::ios
{
    class SwitchCore : public ViewCore, virtual public bdn::SwitchCore
    {
      private:
        static BdnIosSwitchComposite *createSwitchComposite();

      public:
        static std::shared_ptr<SwitchCore> create();

        SwitchCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~SwitchCore() override;

        void init() override;

        void handleClick();

      private:
        BdnIosSwitchComposite *_composite;
        BdnIosSwitchClickManager *_clickManager;
    };
}
