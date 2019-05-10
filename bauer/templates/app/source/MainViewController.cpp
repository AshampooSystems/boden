#include <bdn/ui.h>
#include <bdn/ui/yoga.h>

#include "MainViewController.h"

MainViewController::MainViewController()
{
    _window = std::make_shared<bdn::ui::Window>();
    _window->title = "{project_name}";
    _window->geometry = bdn::Rect{0, 0, 400, 300};
    _window->setLayout(std::make_shared<bdn::ui::yoga::Layout>());

    auto button = std::make_shared<bdn::ui::Button>();
    button->label = "Hello World";

    _window->contentView = button;

    _window->visible = true;
}
