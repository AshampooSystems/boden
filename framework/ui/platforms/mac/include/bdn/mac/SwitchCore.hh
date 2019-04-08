#pragma once

#include <bdn/Switch.h>

#import <bdn/mac/MacSwitch.hh>
#import <bdn/mac/ViewCore.hh>

namespace bdn::mac
{
    class SwitchCore;
}

@interface BdnSwitchClickManager : NSObject
@property std::weak_ptr<bdn::mac::SwitchCore> switchCore;
@end

@interface BdnMacSwitchComposite : NSView
@property(strong) NSTextField *label;
@property(strong) BdnMacSwitch *bdnSwitch;
@end

namespace bdn::mac
{
    class SwitchCore : public ViewCore, virtual public bdn::Switch::Core
    {
      private:
        static BdnMacSwitchComposite *_createSwitchComposite();

      public:
        SwitchCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~SwitchCore();

        void init() override;

        void updateOn();

      private:
        BdnSwitchClickManager *_clickManager;
    };
}
