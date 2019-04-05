#pragma once

#include <bdn/Application.h>

namespace bdn
{
    namespace ios
    {
        int appEntry(const Application::ApplicationControllerFactory &appControllerCreator, int argc, char *argv[]);
    }
}
