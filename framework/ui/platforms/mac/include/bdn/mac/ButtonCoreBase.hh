#ifndef BDN_MAC_ButtonCoreBase_HH_
#define BDN_MAC_ButtonCoreBase_HH_

#include <bdn/ClickEvent.h>

#import <bdn/mac/ChildViewCore.hh>

#import <bdn/mac/util.hh>

namespace bdn
{
    namespace mac
    {

        class ButtonCoreBase : public ChildViewCore
        {
          public:
            ButtonCoreBase(View *pOuter, NSButton *nsButton)
                : ChildViewCore(pOuter, nsButton)
            {
                _nsButton = nsButton;
            }

            void setLabel(const String &label)
            {
                NSString *macLabel = stringToMacString(label);
                [_nsButton setTitle:macLabel];
            }

          protected:
            double getFontSize() const override
            {
                return _nsButton.font.pointSize;
            }

            NSButton *_nsButton;
        };
    }
}

#endif
