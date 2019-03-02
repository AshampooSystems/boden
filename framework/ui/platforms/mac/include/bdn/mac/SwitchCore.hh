#pragma once

#include <bdn/CheckboxCore.h>
#include <bdn/Switch.h>
#include <bdn/SwitchCore.h>

#import <bdn/mac/ChildViewCore.hh>
#import <bdn/mac/MacSwitch.hh>

namespace bdn
{
    namespace mac
    {
        class SwitchCore;

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

        class SwitchCore : public ChildViewCore, virtual public bdn::SwitchCore, virtual public IClickManagerTarget
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
            SwitchCore(std::shared_ptr<Switch> outer) : ChildViewCore(outer, _createSwitchComposite())
            {
                _clickManager = [[BdnSwitchClickManager alloc] init];
                _clickManager.target = this;

                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
                [composite.bdnSwitch setTarget:_clickManager];
                [composite.bdnSwitch setAction:@selector(clicked)];

                setLabel(outer->label);
                setOn(outer->on);
            }

            virtual ~SwitchCore()
            {
                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
                [composite.bdnSwitch setTarget:nil];
                [composite.bdnSwitch setAction:nil];
            }

            void setLabel(const String &label) override
            {
                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
                composite.label.stringValue = stringToNSString(label);
                NSTextFieldCell *cell = [[NSTextFieldCell alloc] initTextCell:composite.label.stringValue];
                [composite.label setFrameSize:cell.cellSize];
            }

            void setOn(const bool &on) override
            {
                BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
                [composite.bdnSwitch setOn:on animate:NO];
            }

            void clicked() override
            {
                std::shared_ptr<Switch> outer = std::dynamic_pointer_cast<Switch>(outerView());
                if (outer != nullptr) {
                    bdn::ClickEvent evt(outer);

                    BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
                    outer->on = composite.bdnSwitch.on;
                    outer->onClick().notify(evt);
                }
            }

          private:
            BdnSwitchClickManager *_clickManager;
        };
    }
}
