#pragma once

#include <functional>

#include <bdn/AppControllerBase.h>

namespace bdn
{
    namespace mac
    {

        /** Entry point function for mac apps with a graphical user interface.
            This is automatically called by the BDN_APP_INIT macro and it
           usually does not need to be called manually.*/
        int uiAppEntry(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator, int argc,
                       char *argv[]);
    }
}