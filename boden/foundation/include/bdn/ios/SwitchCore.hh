#ifndef BDN_IOS_SwitchCore_HH_
#define BDN_IOS_SwitchCore_HH_

#include <bdn/ISwitchCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/Toggle.h>
#include <bdn/Switch.h>

#import <bdn/ios/ToggleCoreBase.hh>

@interface BdnIosSwitchComposite : BdnIosToggleCompositeBase

@property (strong) UISwitch* uiSwitch;

@end

namespace bdn
{
namespace ios
{
    template <class T>
    class SwitchCore : public ToggleCoreBase, BDN_IMPLEMENTS ISwitchCore
    {
    private:
        static BdnIosSwitchComposite* _createSwitchComposite()
        {
            BdnIosSwitchComposite* switchComposite = [[BdnIosSwitchComposite alloc] initWithFrame:CGRectMake(0,0,0,0)];
            
            switchComposite.uiSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(0,0,0,0)];
            [switchComposite addSubview:switchComposite.uiSwitch];
            
            switchComposite.uiLabel = [[UILabel alloc] initWithFrame:CGRectMake(0,0,0,0)];
            [switchComposite addSubview:switchComposite.uiLabel];
            
            return switchComposite;
        }        
        
    public:
        SwitchCore(T* pOuter)
            : ToggleCoreBase(pOuter, _createSwitchComposite())
        {
            BdnIosSwitchComposite* switchComposite = (BdnIosSwitchComposite*)_composite;
            [switchComposite.uiSwitch addTarget:_clickManager
                                         action:@selector(clicked)
                               forControlEvents:UIControlEventTouchUpInside];
            
            // Set initial state
            setLabel(pOuter->label());
            setOn(pOuter->on());
        }

        virtual ~SwitchCore()
        {
            BdnIosSwitchComposite* switchComposite = (BdnIosSwitchComposite*)_composite;
            [switchComposite.uiSwitch removeTarget:_clickManager
                                            action:nil
                                  forControlEvents:UIControlEventTouchUpInside];
        }
        
        void setOn(const bool& on) override
        {
            ((BdnIosSwitchComposite*)_composite).uiSwitch.on = on;
        }

       
        void _clicked() override
        {
            P<View> pView = getOuterViewIfStillAttached();
            if (pView != nullptr) {
                ClickEvent evt(pView);
                
                // Observing the UISwitch state via KVO does not work when
                // the switch state is changed via user interaction. KVO
                // works though when state is set programatically, which
                // unfortunately is useless in the case that a user changes
                // the switch state. This means we have to stick to the
                // click event to propagate the state change to the framework.
                //
                // We guarantee that the on property will be set before
                // a notification is posted to onClick.
                cast<T>(pView)->setOn( ((BdnIosSwitchComposite*)_composite).uiSwitch.on );
                cast<T>(pView)->onClick().notify(evt);
            }
        }
        
        void layout() override
        {
            BdnIosSwitchComposite* switchComposite = (BdnIosSwitchComposite*)_composite;
            
            CGRect compositeBounds = switchComposite.bounds;
            CGRect switchBounds= switchComposite.uiSwitch.bounds;
            CGRect labelBounds = switchComposite.uiLabel.bounds;
            
            // Center switch vertically in composite
            CGRect switchFrame = CGRectMake(compositeBounds.size.width - switchBounds.size.width, compositeBounds.size.height / 2. - switchBounds.size.height / 2., switchBounds.size.width, switchBounds.size.height);
            switchComposite.uiSwitch.frame = switchFrame;
            
            // Center label vertically in composite
            CGRect labelFrame = CGRectMake(0, compositeBounds.size.height / 2. - labelBounds.size.height / 2., labelBounds.size.width, labelBounds.size.height);
            switchComposite.uiLabel.frame = labelFrame;
        }

    };
    
}
}

#endif
