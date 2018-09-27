#include <bdn/init.h>
#include <bdn/IAppRunner.h>

namespace bdn
{

    BDN_SAFE_STATIC(P<IAppRunner>, _getAppRunnerRef);
    BDN_SAFE_STATIC_IMPL(P<IAppRunner>, _getAppRunnerRef);

    P<IAppRunner> getAppRunner() { return _getAppRunnerRef(); }

    void _setAppRunner(IAppRunner *pAppRunner)
    {
        _getAppRunnerRef() = pAppRunner;
    }
}
