#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#include <bdn/mainThread.h>

#include <windows.h>

namespace bdn
{

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
	callFromMainThread(
		[exitCode]()
		{
			::PostQuitMessage(exitCode);
		} );
}

}

