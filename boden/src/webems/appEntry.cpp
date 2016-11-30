#include <bdn/init.h>
#include <bdn/webems/appEntry.h>

#include <bdn/webems/AppRunner.h>

namespace bdn
{
namespace webems
{


int appEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, int argc, char* argv[])
{
    BDN_ENTRY_BEGIN;

    bdn::P< bdn::webems::AppRunner > pAppRunner = bdn::newObj< bdn::webems::AppRunner >( appControllerCreator, argc, argv );
    _setAppRunner(pAppRunner);
    
    return pAppRunner->entry();

    BDN_ENTRY_END(false);
}


}
}


