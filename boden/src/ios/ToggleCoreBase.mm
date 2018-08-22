#include <bdn/init.h>
#include <bdn/ios/ToggleCoreBase.hh>

@interface BdnIosToggleClickManager : NSObject

@property bdn::ios::ToggleCoreBase* pCore;

@end


@implementation BdnIosToggleClickManager

- (void)setToggleCore:(bdn::ios::ToggleCoreBase*)pCore
{
    _pCore = pCore;
}

- (void)clicked
{
    _pCore->_clicked();
}

@end


@implementation BdnIosToggleCompositeBase

@end


namespace bdn
{
namespace ios
{

ToggleCoreBase::ToggleCoreBase(View *pOuter, BdnIosToggleCompositeBase* pToggleComposite)
    : ViewCore(pOuter, pToggleComposite)
{
    _composite = (BdnIosToggleCompositeBase*)getUIView();
    
    BdnIosToggleClickManager* clickManager = [[BdnIosToggleClickManager alloc] init];
    [clickManager setToggleCore:this];
    _clickManager = clickManager;
 }

ToggleCoreBase::~ToggleCoreBase()
{
}

void ToggleCoreBase::setLabel(const String& label)
{
    _composite.uiLabel.text = stringToIosString(label);
    [_composite.uiLabel sizeToFit];
    needLayout(View::InvalidateReason::childSizingInfoInvalidated);
}

}
}
