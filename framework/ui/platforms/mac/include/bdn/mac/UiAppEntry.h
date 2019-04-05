#pragma once

#include <bdn/ApplicationController.h>

namespace bdn
{
    namespace mac
    {

        /** Entry point function for mac apps with a graphical user interface.
            This is automatically called by the BDN_APP_INIT macro and it
           usually does not need to be called manually.*/
        int uiAppEntry(const Application::ApplicationControllerFactory &appControllerCreator, int argc, char *argv[]);
    }
}
