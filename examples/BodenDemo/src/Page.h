#pragma once

#include <bdn/ui.h>
#include <bdn/ui/yoga.h>
#include <functional>

using namespace bdn::ui;
using namespace bdn::ui::yoga;

namespace bdn
{
    std::shared_ptr<View> makeRow(String title, std::shared_ptr<View> ctrl, double marginTop = 5.,
                                  double marginBottom = 5., double contentRatio = 0.62);
    std::shared_ptr<View> createPageContainer(std::shared_ptr<View> page, String nextTitle,
                                              std::function<void()> nextFunc);
}
