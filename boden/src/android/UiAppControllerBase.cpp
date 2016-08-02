#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#include <bdn/mainThread.h>


namespace bdn
{

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
	// android apps should never exit on their own. So, do nothing here.
    int x=0;
    x++;
}

}

