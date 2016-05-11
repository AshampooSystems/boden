#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#import <UiKit/UiKit.h>

namespace bdn
{

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
    [ [UIApplication sharedApplication] performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
}


}


