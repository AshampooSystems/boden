#pragma once

#include <bdn/ui/Switch.h>

#import <bdn/mac/MacSwitch.hh>
#import <bdn/mac/ViewCore.hh>

namespace bdn::ui::mac
{
    class SwitchCore;
}

@interface BdnSwitchClickManager : NSObject
@property std::weak_ptr<bdn::ui::mac::SwitchCore> switchCore;
@end

@interface BdnMacSwitchComposite : NSView
@property(strong) NSTextField *label;
@property(strong) BdnMacSwitch *bdnSwitch;
@end

namespace bdn::ui::mac
{
    class SwitchCore : public ViewCore, virtual public Switch::Core
    {
      private:
        static BdnMacSwitchComposite *_createSwitchComposite();

      public:
        SwitchCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~SwitchCore();

        void init() override;

        void updateOn();

      private:
        BdnSwitchClickManager *_clickManager;
    };
}
