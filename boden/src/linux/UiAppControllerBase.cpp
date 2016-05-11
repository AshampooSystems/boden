#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#include <bdn/mainThread.h>

#include <gtk/gtk.h>

namespace bdn
{

extern int _uiAppExitCode;

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
	callFromMainThread(
		[exitCode]()
		{
            _uiAppExitCode = exitCode;
			gtk_main_quit();
		} );
}

}

