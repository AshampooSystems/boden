#include <bdn/init.h>
#include <bdn/IDispatcher.h>
#include <bdn/IAppRunner.h>

namespace bdn
{

    P<IDispatcher> getMainDispatcher() { return getAppRunner()->getMainDispatcher(); }
}
