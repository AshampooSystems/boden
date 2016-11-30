#ifndef BDN_WIN32_appEntry_H_
#define BDN_WIN32_appEntry_H_

namespace bdn
{
namespace win32
{
    

/** Entry point function for command line win32 apps.
    This is automatically called by the BDN_APP_INIT macro and it usually
    does not need to be called manually.*/
int commandLineAppEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, int argc, char* argv[]);


/** Entry point function for win32 apps with a graphical user interface.
    This is automatically called by the BDN_APP_INIT macro and it usually
    does not need to be called manually.*/
int uiAppEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, int showCommand);


}
}

#endif

