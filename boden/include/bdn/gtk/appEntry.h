#ifndef BDN_GTK_appEntry_H_
#define BDN_GTK_appEntry_H_

#include <bdn/AppControllerBase.h>

namespace bdn
{
namespace gtk
{
    

/** Entry point function for GTK apps with a graphical user interface.
    This is automatically called by the BDN_APP_INIT macro and it usually
    does not need to be called manually.*/
int uiAppEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, int argc, char* argv[]);


}
}

#endif

