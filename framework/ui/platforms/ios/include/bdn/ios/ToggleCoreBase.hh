#pragma once

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
        class ToggleCoreBase : public ViewCore, virtual public IToggleCoreBase
        {
          public:
            ToggleCoreBase(std::shared_ptr<View> outer, BdnIosToggleCompositeBase *toggleComposite);
            virtual ~ToggleCoreBase();

            void setLabel(const String &label) override;

            virtual void _clicked() = 0;

          protected:
            BdnIosToggleCompositeBase *_composite;
            BdnIosToggleClickManager *_clickManager;
        };
    }
}
