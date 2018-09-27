#include <bdn/init.h>
#include <bdn/mac/ToggleCoreBase.hh>

#import <Cocoa/Cocoa.h>

// FIXME: duplicated from ButtonCore.mm
@interface BdnToggleClickManager : NSObject
@property bdn::mac::ToggleCoreBase *pToggleCoreBase;
@end

@implementation BdnToggleClickManager

- (void)setToggleCoreBase:(bdn::mac::ToggleCoreBase *)pCore
{
    _pToggleCoreBase = pCore;
}

- (void)clicked { _pToggleCoreBase->generateClick(); }

@end

namespace bdn
{
    namespace mac
    {

        ToggleCoreBase::ToggleCoreBase(View *pOuterToggle)
            : ButtonCoreBase(pOuterToggle, _createNsButton())
        {
            BdnToggleClickManager *clickMan =
                [[BdnToggleClickManager alloc] init];
            [clickMan setToggleCoreBase:this];
            _clickManager = clickMan;

            [_nsButton setTarget:clickMan];
            [_nsButton setAction:@selector(clicked)];
        }
    }
}
