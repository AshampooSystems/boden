#include <bdn/init.h>
#include <bdn/mac/ToggleCoreBase.hh>

#import <Cocoa/Cocoa.h>

// FIXME: duplicated from ButtonCore.mm
@interface BdnToggleClickManager : NSObject
@property bdn::mac::ToggleCoreBase *toggleCoreBase;
@end

@implementation BdnToggleClickManager

- (void)setToggleCoreBase:(bdn::mac::ToggleCoreBase *)core { _toggleCoreBase = core; }

- (void)clicked { _toggleCoreBase->generateClick(); }

@end

namespace bdn
{
    namespace mac
    {

        ToggleCoreBase::ToggleCoreBase(View *outerToggle) : ButtonCoreBase(outerToggle, _createNsButton())
        {
            BdnToggleClickManager *clickMan = [[BdnToggleClickManager alloc] init];
            [clickMan setToggleCoreBase:this];
            _clickManager = clickMan;

            [_nsButton setTarget:clickMan];
            [_nsButton setAction:@selector(clicked)];
        }
    }
}
