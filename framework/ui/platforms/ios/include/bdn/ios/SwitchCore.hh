#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/Switch.h>
#include <bdn/SwitchCore.h>

#import <bdn/ios/ViewCore.hh>

#import <UIKit/UIKit.h>

@interface BdnIosSwitchComposite : UIControl

@property(strong) UISwitch *uiSwitch;
@property(strong) UILabel *uiLabel;

@end

@interface BdnIosSwitchClickManager : NSObject

@property(nonatomic, assign) std::weak_ptr<bdn::Switch> outer;

@end

namespace bdn
{
    namespace ios
    {
        class SwitchCore : public ViewCore, virtual public bdn::SwitchCore
        {
          private:
            static BdnIosSwitchComposite *createSwitchComposite();

          public:
            static std::shared_ptr<SwitchCore> create(std::shared_ptr<Switch> outer);

            SwitchCore(std::shared_ptr<Switch> outer);
            virtual ~SwitchCore();

            void setOn(const bool &on) override;
            void setLabel(const String &label) override;

            void layout() override;

          private:
            BdnIosSwitchComposite *_composite;
            BdnIosSwitchClickManager *_clickManager;
        };
    }
}
