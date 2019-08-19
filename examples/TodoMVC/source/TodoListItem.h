#pragma once

#include <bdn/platform.h>
#include <bdn/ui/Button.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/CoreLess.h>
#include <bdn/ui/TriState.h>

class TodoListItem : public bdn::ui::CoreLess<bdn::ui::ContainerView>
{
  public:
    bdn::Property<std::string> text;
    bdn::Property<bool> completed;

  public:
    using CoreLess<ContainerView>::CoreLess;

  public:
    void init() override;

#ifdef BDN_PLATFORM_OSX
  public:
    std::shared_ptr<bdn::ui::Button> _deleteButton;
#endif
};
