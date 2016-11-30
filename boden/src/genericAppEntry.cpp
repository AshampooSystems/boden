#include <bdn/init.h>
#include <bdn/genericAppEntry.h>

#include <bdn/entry.h>
#include <bdn/GenericAppRunner.h>

namespace bdn
{

int genericCommandLineAppEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, int argc, char* argv[])
{
    BDN_ENTRY_BEGIN;

    bdn::P< GenericAppRunner > pAppRunner = bdn::newObj< GenericAppRunner >( appControllerCreator, argc, argv );
    _setAppRunner(pAppRunner);
    
    pAppRunner->entry();

    BDN_ENTRY_END(false);
    
    return 0;
}



}




