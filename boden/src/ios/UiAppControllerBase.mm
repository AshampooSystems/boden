#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#import <UiKit/UiKit.h>

namespace bdn
{

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
    // ios apps cannot close themselves. So we do nothing here.
    int x=0;
    x++;
}


}


