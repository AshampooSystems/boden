#pragma once

#include <bdn/ApplicationController.h>

namespace bdn
{
    namespace ios
    {
        int appEntry(const std::function<std::shared_ptr<ApplicationController>()> &appControllerCreator, int argc,
                     char *argv[]);
    }
}
