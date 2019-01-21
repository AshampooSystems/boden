#pragma once

#include <bdn/ISwitchCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/Toggle.h>
#include <bdn/Switch.h>

#import <bdn/ios/ToggleCoreBase.hh>

@interface BdnIosSwitchComposite : BdnIosToggleCompositeBase

@property(strong) UISwitch *uiSwitch;

@end

namespace bdn
{
    namespace ios
    {
        template <class T> class SwitchCore : public ToggleCoreBase, virtual public ISwitchCore
        {
          private:
            static BdnIosSwitchComposite *_createSwitchComposite()
            {
                BdnIosSwitchComposite *switchComposite =
                    [[BdnIosSwitchComposite alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];

                switchComposite.uiSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
                [switchComposite addSubview:switchComposite.uiSwitch];

                switchComposite.uiLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
                [switchComposite addSubview:switchComposite.uiLabel];

                return switchComposite;
            }

          public:
            static std::shared_ptr<SwitchCore<T>> create(std::shared_ptr<T> outer);

            SwitchCore(std::shared_ptr<T> outer) : ToggleCoreBase(outer, _createSwitchComposite())
            {
                BdnIosSwitchComposite *switchComposite = (BdnIosSwitchComposite *)_composite;
                [switchComposite.uiSwitch addTarget:_clickManager
                                             action:@selector(clicked)
                                   forControlEvents:UIControlEventTouchUpInside];

                // Set initial state
                setLabel(outer->label);
                setOn(outer->on);
            }

            virtual ~SwitchCore()
            {
                BdnIosSwitchComposite *switchComposite = (BdnIosSwitchComposite *)_composite;
                [switchComposite.uiSwitch removeTarget:_clickManager
                                                action:nil
                                      forControlEvents:UIControlEventTouchUpInside];
            }

            void setOn(const bool &on) override { ((BdnIosSwitchComposite *)_composite).uiSwitch.on = on; }

            void _clicked() override
            {
                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    ClickEvent evt(view);

                    // Observing the UISwitch state via KVO does not work when
                    // the switch state is changed via user interaction. KVO
                    // works though when state is set programatically, which
                    // unfortunately is useless in the case that a user changes
                    // the switch state. This means we have to stick to the
                    // click event to propagate the state change to the
                    // framework.
                    //
                    // We guarantee that the on property will be set before
                    // a notification is posted to onClick.
                    std::dynamic_pointer_cast<T>(view)->on = (((BdnIosSwitchComposite *)_composite).uiSwitch.on);
                    std::dynamic_pointer_cast<T>(view)->onClick().notify(evt);
                }
            }

            void layout() override
            {
                BdnIosSwitchComposite *switchComposite = (BdnIosSwitchComposite *)_composite;

                CGRect compositeBounds = switchComposite.bounds;
                CGRect switchBounds = switchComposite.uiSwitch.bounds;
                CGRect labelBounds = switchComposite.uiLabel.bounds;

                // Center switch vertically in composite
                CGRect switchFrame = CGRectMake(compositeBounds.size.width - switchBounds.size.width,
                                                compositeBounds.size.height / 2. - switchBounds.size.height / 2.,
                                                switchBounds.size.width, switchBounds.size.height);
                switchComposite.uiSwitch.frame = switchFrame;

                // Center label vertically in composite
                CGRect labelFrame = CGRectMake(0, compositeBounds.size.height / 2. - labelBounds.size.height / 2.,
                                               labelBounds.size.width, labelBounds.size.height);
                switchComposite.uiLabel.frame = labelFrame;
            }
        };
    }
}
