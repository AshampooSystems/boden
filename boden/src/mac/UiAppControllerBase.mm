#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#import <Cocoa/Cocoa.h>

namespace bdn
{

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
    [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
}


}


