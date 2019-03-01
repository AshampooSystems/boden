#include <bdn/ui.h>
#include <bdn/yogalayout.h>

#include "MainViewController.h"

MainViewController::MainViewController()
{
    _window = std::make_shared<bdn::Window>();
    _window->title = "{project_name}";
    _window->geometry = bdn::Rect{0, 0, 400, 300};
    _window->setLayout(std::make_shared<bdn::yogalayout::Layout>());

    std::shared_ptr<bdn::Button> button = std::make_shared<bdn::Button>();
    button->label = "Hello World";

    _window->content = button;

    _window->visible = true;
}
