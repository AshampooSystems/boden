#include <bdn/Button.h>

#include "MainViewController.h"

MainViewController::MainViewController()
{
    _window = std::make_shared<bdn::Window>();
    _window->title = "{project_name}";

    std::shared_ptr<bdn::Button> button = std::make_shared<bdn::Button>();
    button->label = "Hello World";

    _window->setContentView(button);
    _window->requestAutoSize();
    _window->requestCenter();

    _window->visible = true;
}
