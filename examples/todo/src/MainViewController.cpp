#include <bdn/init.h>

#include "MainViewController.h"
#include "ViewModel.h"

MainViewController::MainViewController(ViewModel *viewModel)
    : _viewModel(viewModel)
{
    _window = bdn::newObj<bdn::Window>();

    _textView = bdn::newObj<bdn::TextView>();
    _textView->setText("Hello World");

    _window->setContentView(_textView);
    _window->requestAutoSize();
    _window->requestCenter();
    _window->setVisible(true);
}
