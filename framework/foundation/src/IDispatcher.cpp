
#include <bdn/AppRunnerBase.h>
#include <bdn/IDispatcher.h>

namespace bdn
{

    std::shared_ptr<IDispatcher> getMainDispatcher() { return getAppRunner()->getMainDispatcher(); }
}
