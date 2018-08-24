#include <bdn/init.h>
#include <bdn/AppControllerBase.h>

#include <bdn/log.h>

namespace bdn
{

void AppControllerBase::unhandledProblem(IUnhandledProblem& problem)
{
    // log the problem and let the app terminate.

    logError( "TERMINATING app because of unhandled problem: "+problem.toString() );
}


}

