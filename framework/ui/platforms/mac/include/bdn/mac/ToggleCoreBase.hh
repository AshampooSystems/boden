#ifndef BDN_MAC_ToggleCoreBase_HH
#define BDN_MAC_ToggleCoreBase_HH

#include <bdn/constants.h>
#include <bdn/IToggleCoreBase.h>
#import <bdn/mac/ButtonCoreBase.hh>

@class BdnToggleClickManager;

namespace bdn
{
    namespace mac
    {

        /** Base class for all toggle-like control cores on macOS */
        class ToggleCoreBase : public ButtonCoreBase, BDN_IMPLEMENTS IToggleCoreBase
        {
          private:
            static NSButton *_createNsButton()
            {
                NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                [button setButtonType:NSButtonTypeSwitch];
                return button;
            }

          public:
            ToggleCoreBase(View *pOuterToggle);

            virtual void setLabel(const String &label) override { ButtonCoreBase::setLabel(label); }

            virtual void generateClick() = 0;

          private:
            BdnToggleClickManager *_clickManager;
        };
    }
}

#endif /* ToggleCoreBase_h */
