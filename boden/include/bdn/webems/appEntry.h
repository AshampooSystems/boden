#ifndef BDN_WEBEMS_appEntry_H_
#define BDN_WEBEMS_appEntry_H_


namespace bdn
{
namespace webems
{
    

/** Entry point function for webems apps.
    This is automatically called by the BDN_APP_INIT macro and it usually
    does not need to be called manually.*/
int appEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, int argc, char* argv[]);


}
}

#endif

