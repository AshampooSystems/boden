#include <bdn/init.h>
#include <bdn/winuwp/appEntry.h>

#include <bdn/winuwp/AppRunner.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace winuwp
    {

        int appEntry(
            const std::function<P<AppControllerBase>()> &appControllerCreator,
            Platform::Array<Platform::String ^> ^ args)
        {
            BDN_ENTRY_BEGIN;

            bdn::P<bdn::winuwp::AppRunner> pAppRunner =
                bdn::newObj<bdn::winuwp::AppRunner>(appControllerCreator, args);
            _setAppRunner(pAppRunner);
            return pAppRunner->entry();
            BDN_ENTRY_END(false);
            return 0;
        }
    }
}
