#ifndef BDN_MAC_SwitchCore_HH
#define BDN_MAC_SwitchCore_HH

#include <bdn/ICheckboxCore.h>
#include <bdn/ISwitchCore.h>
#include <bdn/Switch.h>
#include <bdn/Toggle.h>

#import <bdn/mac/MacSwitch.hh>
#import <bdn/mac/ToggleCoreBase.hh>

namespace bdn
{
    namespace mac
    {
        template <class T> class SwitchCore;

        class IClickManagerTarget
        {
          public:
            virtual void clicked() = 0;
        };
    }
}

@interface BdnSwitchClickManager : NSObject

@property bdn::mac::IClickManagerTarget *pTarget;

@end

@interface BdnMacSwitchComposite : NSView

@property(strong) NSTextField *label;
@property(strong) BdnMacSwitch *bdnSwitch;

@end

namespace bdn
{
    namespace mac
    {

        template <class T>
        class SwitchCore : public ChildViewCore,
                           BDN_IMPLEMENTS ISwitchCore,
                           BDN_IMPLEMENTS IClickManagerTarget
        {
          private:
            static BdnMacSwitchComposite *_createSwitchComposite()
            {
                BdnMacSwitchComposite *switchComposite =
                    [[BdnMacSwitchComposite alloc]
                        initWithFrame:NSMakeRect(0, 0, 0, 0)];

                switchComposite.bdnSwitch =
                    [[BdnMacSwitch alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                [switchComposite addSubview:switchComposite.bdnSwitch];

                switchComposite.label =
                    [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                switchComposite.label.bezeled = NO;
                switchComposite.label.drawsBackground = NO;
                switchComposite.label.editable = NO;
                switchComposite.label.frame = CGRectMake(0, 0, 100, 20);
                [switchComposite addSubview:switchComposite.label];

                return switchComposite;
            }

          public:
            SwitchCore(T *pOuter)
                : ChildViewCore(pOuter, _createSwitchComposite())
            {
                _clickManager = [[BdnSwitchClickManager alloc] init];
                _clickManager.pTarget = this;

                BdnMacSwitchComposite *composite =
                    (BdnMacSwitchComposite *)getNSView();
                [composite.bdnSwitch setTarget:_clickManager];
                [composite.bdnSwitch setAction:@selector(clicked)];

                setLabel(pOuter->label());
                setOn(pOuter->on());
            }

            virtual ~SwitchCore()
            {
                BdnMacSwitchComposite *composite =
                    (BdnMacSwitchComposite *)getNSView();
                [composite.bdnSwitch setTarget:nil];
                [composite.bdnSwitch setAction:nil];
            }

            void setLabel(const String &label) override
            {
                BdnMacSwitchComposite *composite =
                    (BdnMacSwitchComposite *)getNSView();
                composite.label.stringValue = stringToMacString(label);
                NSTextFieldCell *cell = [[NSTextFieldCell alloc]
                    initTextCell:composite.label.stringValue];
                [composite.label setFrameSize:cell.cellSize];
            }

            // Called when attached to a Switch or Toggle with switch appearance
            void setOn(const bool &on) override
            {
                BdnMacSwitchComposite *composite =
                    (BdnMacSwitchComposite *)getNSView();
                [composite.bdnSwitch setOn:on animate:NO];
            }

            void clicked() override
            {
                P<T> pOuter = cast<T>(getOuterViewIfStillAttached());
                if (pOuter != nullptr) {
                    bdn::ClickEvent evt(pOuter);

                    BdnMacSwitchComposite *composite =
                        (BdnMacSwitchComposite *)getNSView();
                    pOuter->setOn(composite.bdnSwitch.on);
                    pOuter->onClick().notify(evt);
                }
            }

            void layout() override
            {
                BdnMacSwitchComposite *composite =
                    (BdnMacSwitchComposite *)getNSView();

                CGRect compositeBounds = composite.bounds;
                CGRect switchBounds = composite.bdnSwitch.bounds;
                CGRect labelBounds = composite.label.bounds;

                // Center switch vertically in composite
                CGRect switchFrame = CGRectMake(
                    compositeBounds.size.width - switchBounds.size.width,
                    compositeBounds.size.height / 2. -
                        switchBounds.size.height / 2.,
                    switchBounds.size.width, switchBounds.size.height);
                composite.bdnSwitch.frame = switchFrame;

                // Center label vertically in composite
                CGRect labelFrame =
                    CGRectMake(0,
                               compositeBounds.size.height / 2. -
                                   labelBounds.size.height / 2.,
                               labelBounds.size.width, labelBounds.size.height);
                composite.label.frame = labelFrame;
            }

          private:
            BdnSwitchClickManager *_clickManager;
        };
    }
}

#endif /* BDN_MAC_SwitchCore_HH */
