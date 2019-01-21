#pragma once

#include <bdn/ICheckboxCore.h>
#include <bdn/ISwitchCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/Checkbox.h>

#import <bdn/ios/ToggleCoreBase.hh>
#import <bdn/ios/IosCheckbox.hh>

@interface BdnIosCheckboxComposite : BdnIosToggleCompositeBase

@property(strong) BdnIosCheckbox *checkbox;

@end

namespace bdn
{
    namespace ios
    {

        class CheckboxCore : public ToggleCoreBase, virtual public ICheckboxCore, virtual public ISwitchCore
        {
          private:
            static BdnIosCheckboxComposite *_createCheckboxComposite();

          public:
            CheckboxCore(std::shared_ptr<Checkbox> outerCheckbox);
            virtual ~CheckboxCore();

            void setState(const TriState &state) override;

            void setOn(const bool &on) override;

            void _clicked() override;

            void layout() override;
        };
    }
}
