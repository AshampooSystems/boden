#pragma once

#include <bdn/Application.h>

namespace bdn::ui::ios
{
    int appEntry(const Application::ApplicationControllerFactory &appControllerCreator, int argc, char *argv[]);
}
