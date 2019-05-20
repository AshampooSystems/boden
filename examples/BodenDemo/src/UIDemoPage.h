#pragma once

#include <bdn/ui.h>

#include "Page.h"

namespace bdn
{
    std::shared_ptr<ui::ContainerView> createUiDemoPage(std::shared_ptr<ui::Window> window);
}
