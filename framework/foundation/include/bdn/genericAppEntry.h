#pragma once

#include <bdn/Application.h>

namespace bdn
{

    /** A generic implementation for a commandline app entry function.
        You do not normally have to call this directly. use the BDN_APP_INIT
       macro instead.*/
    int genericCommandLineAppEntry(const Application::ApplicationControllerFactory &appControllerCreator, int argc,
                                   char *argv[]);
}
