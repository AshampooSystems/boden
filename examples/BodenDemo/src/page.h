#pragma once

#include <bdn/Button.h>
#include <bdn/ContainerView.h>
#include <bdn/NavigationView.h>
#include <bdn/View.h>
#include <bdn/Window.h>

#include <functional>

namespace bdn
{
    std::shared_ptr<View> createPageContainer(std::shared_ptr<View> page, String nextTitle,
                                              std::function<void()> nextFunc)
    {
        auto mainContainer = std::make_shared<ContainerView>();
        mainContainer->stylesheet =
            FlexJsonStringify({"flexGrow" : 1.0, "padding" : {"all" : 10.0}, "margin" : {"all" : 2.0}});

        mainContainer->addChildView(page);

        auto nextButton = std::make_shared<Button>();
        nextButton->label = nextTitle + " > ";
        nextButton->stylesheet =
            FlexJsonStringify({"flexShrink" : 0.0, "alignSelf" : "FlexEnd", "margin" : {"top" : 5}});

        nextButton->onClick() += [=](auto) { nextFunc(); };

        mainContainer->addChildView(nextButton);

        return mainContainer;
    }
}
