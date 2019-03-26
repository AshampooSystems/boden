#pragma once

#include <bdn/CheckboxCore.h>
#include <bdn/Switch.h>
#include <bdn/SwitchCore.h>

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
    class SwitchCore : public ViewCore, virtual public bdn::SwitchCore
    {
      private:
        static BdnMacSwitchComposite *_createSwitchComposite();

      public:
        SwitchCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~SwitchCore() override;

        void init() override;

      private:
        BdnSwitchClickManager *_clickManager;
    };
}
