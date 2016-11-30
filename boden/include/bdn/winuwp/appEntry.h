#ifndef BDN_WINUWP_appEntry_H_
#define BDN_WINUWP_appEntry_H_

namespace bdn
{
namespace winuwp
{
    

/** Entry point function for winuwp apps.
    This is automatically called by the BDN_APP_INIT macro and it usually
    does not need to be called manually.*/
int appEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, Platform::Array<Platform::String^>^ args);


}
}

#endif

