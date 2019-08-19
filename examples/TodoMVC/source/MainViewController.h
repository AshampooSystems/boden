#pragma once

#include <bdn/ui/ContainerView.h>
#include <bdn/ui/ListView.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/Window.h>

#include "TodoListData.h"

class MainViewController
{
  public:
    MainViewController();
    virtual ~MainViewController() = default;

  private:
    void handleDataEmptyChange();

  private:
    std::shared_ptr<bdn::ui::Window> _window;
    std::shared_ptr<TodoListDataSource> _todoDataSource;
    std::shared_ptr<bdn::ui::TextField> _newEntryField;
    std::shared_ptr<bdn::ui::ListView> _listView;
    std::shared_ptr<bdn::ui::ContainerView> _mainContainer;
};
