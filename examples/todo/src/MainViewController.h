#pragma once

#include <bdn/Base.h>
#include <bdn/Window.h>
#include <bdn/TextView.h>

class ViewModel;

class MainViewController : public bdn::Base
{
  public:
    MainViewController(ViewModel *pViewModel);

  private:
    bdn::P<ViewModel> _viewModel;

  private:
    bdn::P<bdn::Window> _window;
    bdn::P<bdn::TextView> _textView;
};
