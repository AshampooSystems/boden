#pragma once

#include <bdn/ApplicationController.h>

namespace bdn
{

    /** A generic implementation for a commandline app entry function.
        You do not normally have to call this directly. use the BDN_APP_INIT
       macro instead.*/
    int genericCommandLineAppEntry(const std::function<std::shared_ptr<ApplicationController>()> &appControllerCreator,
                                   int argc, char *argv[]);
}
