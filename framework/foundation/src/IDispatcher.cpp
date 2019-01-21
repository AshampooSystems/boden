
#include <bdn/IDispatcher.h>
#include <bdn/AppRunnerBase.h>

namespace bdn
{

    std::shared_ptr<IDispatcher> getMainDispatcher() { return getAppRunner()->getMainDispatcher(); }
}
