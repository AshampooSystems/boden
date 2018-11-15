#ifndef BDN_IOS_appEntry_H_
#define BDN_IOS_appEntry_H_

#include <bdn/AppControllerBase.h>

namespace bdn
{
    namespace ios
    {

        /** Entry point function for iOS apps.
            This is automatically called by the BDN_APP_INIT macro and it
           usually does not need to be called manually.*/
        int appEntry(const std::function<P<AppControllerBase>()> &appControllerCreator, int argc, char *argv[]);
    }
}

#endif
