#include <bdn/init.h>
#include <bdn/UiAppControllerBase.h>

#include <bdn/mainThread.h>

#include <emscripten.h>

namespace bdn
{

void UiAppControllerBase::closeAtNextOpportunityIfPossible(int exitCode)
{
	onTerminate();
	emscripten_force_exit(exitCode);
}

}

