#pragma once

#include <bdn/AppControllerBase.h>

namespace bdn
{
    namespace ios
    {

        /** Entry point function for iOS apps.
            This is automatically called by the BDN_APP_INIT macro and it
           usually does not need to be called manually.*/
        int appEntry(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator, int argc,
                     char *argv[]);
    }
}
