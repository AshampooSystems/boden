#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#include <bdn/mainThread.h>

#include <windows.h>

namespace bdn
{

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
	// UWP apps should never exit themselves.
	// CoreApplication.Exit() and Application.Current.Exit() exist in the API,
	// but the documentation states that they should not be used.
}

}

