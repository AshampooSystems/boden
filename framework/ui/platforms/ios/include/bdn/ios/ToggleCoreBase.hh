#ifndef BDN_IOS_ToggleCoreBase_HH_
#define BDN_IOS_ToggleCoreBase_HH_

#include <bdn/IToggleCoreBase.h>
#include <bdn/ClickEvent.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosToggleClickManager;

@interface BdnIosToggleCompositeBase : UIControl

@property(strong) UILabel *uiLabel;

@end

namespace bdn
{
    namespace ios
    {

        class ToggleCoreBase : public ViewCore, BDN_IMPLEMENTS IToggleCoreBase
        {
          public:
            ToggleCoreBase(View *outer, BdnIosToggleCompositeBase *toggleComposite);
            virtual ~ToggleCoreBase();

            void setLabel(const String &label) override;

            virtual void _clicked() = 0;

          protected:
            BdnIosToggleCompositeBase *_composite;
            BdnIosToggleClickManager *_clickManager;
        };
    }
}

#endif
