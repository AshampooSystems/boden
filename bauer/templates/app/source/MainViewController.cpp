#include <bdn/init.h>
#include <bdn/Button.h>

#include "MainViewController.h"

MainViewController::MainViewController()
{
    _window = bdn::newObj<bdn::Window>();
    _window->setTitle("{project_name}");

    bdn::P<bdn::Button> button = bdn::newObj<bdn::Button>();
    button->setLabel("Hello World");

    _window->setContentView(button);
    _window->requestAutoSize();
    _window->requestCenter();

    _window->setVisible(true);
}
