#pragma once

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

@property bdn::mac::IClickManagerTarget *target;

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
        class SwitchCore : public ChildViewCore, virtual public ISwitchCore, virtual public IClickManagerTarget
        {
          private:
            static BdnMacSwitchComposite *_createSwitchComposite()
            {
                BdnMacSwitchComposite *switchComposite =
                    [[BdnMacSwitchComposite alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

                switchComposite.bdnSwitch = [[BdnMacSwitch alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                [switchComposite addSubview:switchComposite.bdnSwitch];

                switchComposite.label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                switchComposite.label.bezeled = NO;
                switchComposite.label.drawsBackground = NO;
                switchComposite.label.editable = NO;
                switchComposite.label.frame = CGRectMake(0, 0, 100, 20);
                [switchComposite addSubview:switchComposite.label];

                return switchComposite;
            }

          public:
            SwitchCore(std::shared_ptr<T> outer) : ChildViewCore(outer, _createSwitchComposite())
            {
                _clickManager = [[BdnSwitchClickManager alloc] init];
                _clickManager.target = this;

                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)getNSView();
                [composite.bdnSwitch setTarget:_clickManager];
                [composite.bdnSwitch setAction:@selector(clicked)];

                setLabel(outer->label);
                setOn(outer->on);
            }

            virtual ~SwitchCore()
            {
                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)getNSView();
                [composite.bdnSwitch setTarget:nil];
                [composite.bdnSwitch setAction:nil];
            }

            void setLabel(const String &label) override
            {
                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)getNSView();
                composite.label.stringValue = stringToNSString(label);
                NSTextFieldCell *cell = [[NSTextFieldCell alloc] initTextCell:composite.label.stringValue];
                [composite.label setFrameSize:cell.cellSize];
            }

            // Called when attached to a Switch or Toggle with switch appearance
            void setOn(const bool &on) override
            {
                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)getNSView();
                [composite.bdnSwitch setOn:on animate:NO];
            }

            void clicked() override
            {
                std::shared_ptr<T> outer = std::dynamic_pointer_cast<T>(getOuterViewIfStillAttached());
                if (outer != nullptr) {
                    bdn::ClickEvent evt(outer);

                    BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)getNSView();
                    outer->on = composite.bdnSwitch.on;
                    outer->onClick().notify(evt);
                }
            }

            void layout() override
            {
                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)getNSView();

                CGRect compositeBounds = composite.bounds;
                CGRect switchBounds = composite.bdnSwitch.bounds;
                CGRect labelBounds = composite.label.bounds;

                // Center switch vertically in composite
                CGRect switchFrame = CGRectMake(compositeBounds.size.width - switchBounds.size.width,
                                                compositeBounds.size.height / 2. - switchBounds.size.height / 2.,
                                                switchBounds.size.width, switchBounds.size.height);
                composite.bdnSwitch.frame = switchFrame;

                // Center label vertically in composite
                CGRect labelFrame = CGRectMake(0, compositeBounds.size.height / 2. - labelBounds.size.height / 2.,
                                               labelBounds.size.width, labelBounds.size.height);
                composite.label.frame = labelFrame;
            }

          private:
            BdnSwitchClickManager *_clickManager;
        };
    }
}
