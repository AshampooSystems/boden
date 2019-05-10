#pragma once

#include <bdn/Application.h>
#include <functional>

namespace bdn::ui::mac
{
    int uiAppEntry(const Application::ApplicationControllerFactory &appControllerCreator, int argc, char *argv[]);
}
