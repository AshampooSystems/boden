#include <bdn/ui/Button.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/NavigationView.h>
#include <bdn/ui/ScrollView.h>
#include <bdn/ui/Switch.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/Window.h>
#include <bdn/ui/yoga.h>

#include "MainViewController.h"

MainViewController::MainViewController()
{
    _window = std::make_shared<bdn::ui::Window>();
    _window->title = "KeyboardTest";
    _window->geometry = bdn::Rect{0, 0, 400, 300};
    _window->setLayout(std::make_shared<bdn::ui::yoga::Layout>());

    auto btn = std::make_shared<bdn::ui::Button>();
    btn->label = "ScrollView Test";
    btn->onClick() += [this](auto) { _window->contentView = createScrollPage(); };

    _window->contentView = createContent(10, btn);

    _window->visible = true;
}

std::shared_ptr<bdn::ui::ContainerView> MainViewController::createContent(int numItems,
                                                                          std::shared_ptr<bdn::ui::Button> nextButton)
{
    auto container = std::make_shared<bdn::ui::ContainerView>();
    container->stylesheet = FlexJsonStringify({
        "direction" : "Column",
        "justifyContent" : "SpaceBetween",
        "alignItems" : "Stretch",
        "flexGrow" : 1,
        "flexShrink" : 0,
        "padding" : {"left" : 10, "right" : 10, "top" : 100, "bottom" : 10}
    });

    for (int i = 0; i < numItems; i++) {
        if (i == 0) {
            auto label = std::make_shared<bdn::ui::Label>();
            label->text = "Hello World";
            label->stylesheet = FlexJsonStringify({"flexShrink" : 0});

            container->addChildView(label);
        } else if (i == 4 && nextButton) {
            nextButton->stylesheet = FlexJsonStringify({"flexShrink" : 0});
            container->addChildView(nextButton);
        } else {
            auto textField = std::make_shared<bdn::ui::TextField>();
            textField->stylesheet = FlexJsonStringify({"flexShrink" : 0});
            container->addChildView(textField);
        }
    }

    return container;
}

std::shared_ptr<bdn::ui::View> MainViewController::createNavPage()
{
    auto navView = std::make_shared<bdn::ui::NavigationView>();

    auto btn = std::make_shared<bdn::ui::Button>();

    btn->label = "NavigationView + Scroll Test";
    btn->onClick() += [this, navView](auto) {
        auto scrollView = std::make_shared<bdn::ui::ScrollView>();
        scrollView->stylesheet = FlexJsonStringify({"flexGrow" : 1});

        auto content = createContent(50, nullptr);
        scrollView->contentView = content;

        navView->pushView(scrollView, "NavigationView + Scroll");
    };

    navView->pushView(createContent(10, btn), "NavigationView");
    navView->stylesheet = FlexJsonStringify({"flexGrow" : 1});

    return navView;
}

std::shared_ptr<bdn::ui::View> MainViewController::createScrollPage()
{
    auto scrollView = std::make_shared<bdn::ui::ScrollView>();
    scrollView->stylesheet = FlexJsonStringify({"flexGrow" : 1});

    auto btn = std::make_shared<bdn::ui::Button>();
    btn->label = "NavigationView Test";
    btn->onClick() += [this](auto) { _window->contentView = createNavPage(); };

    auto content = createContent(50, btn);
    scrollView->contentView = content;

    return scrollView;
}
