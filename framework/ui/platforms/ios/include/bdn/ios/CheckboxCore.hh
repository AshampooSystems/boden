#ifndef BDN_IOS_CheckboxCore_HH_
#define BDN_IOS_CheckboxCore_HH_

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

        class CheckboxCore : public ToggleCoreBase, BDN_IMPLEMENTS ICheckboxCore, BDN_IMPLEMENTS ISwitchCore
        {
          private:
            static BdnIosCheckboxComposite *_createCheckboxComposite();

          public:
            CheckboxCore(Checkbox *pOuterCheckbox);
            virtual ~CheckboxCore();

            void setState(const TriState &state) override
            {
                ((BdnIosCheckboxComposite *)_composite).checkbox.checkboxState = state;
            }

            void setOn(const bool &on) override { setState(on ? TriState::on : TriState::off); }

            void _clicked() override;

            void layout() override;
        };
    }
}

#endif
