#include <bdn/init.h>
#include <bdn/ios/ToggleCoreBase.hh>

@interface BdnIosToggleClickManager : NSObject

@property bdn::ios::ToggleCoreBase *core;

@end

@implementation BdnIosToggleClickManager

- (void)setToggleCore:(bdn::ios::ToggleCoreBase *)core { _core = core; }

- (void)clicked { _core->_clicked(); }

@end

@implementation BdnIosToggleCompositeBase

@end

namespace bdn
{
    namespace ios
    {

        ToggleCoreBase::ToggleCoreBase(View *outer, BdnIosToggleCompositeBase *toggleComposite)
            : ViewCore(outer, toggleComposite)
        {
            _composite = (BdnIosToggleCompositeBase *)getUIView();

            BdnIosToggleClickManager *clickManager = [[BdnIosToggleClickManager alloc] init];
            [clickManager setToggleCore:this];
            _clickManager = clickManager;
        }

        ToggleCoreBase::~ToggleCoreBase() {}

        void ToggleCoreBase::setLabel(const String &label)
        {
            _composite.uiLabel.text = stringToIosString(label);
            [_composite.uiLabel sizeToFit];
            needLayout(View::InvalidateReason::childSizingInfoInvalidated);
        }
    }
}
