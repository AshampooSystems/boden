#pragma once

#include <bdn/Base.h>

class TodoModel;

class ViewModel : public bdn::Base
{
  public:
    ViewModel(TodoModel *model);

  private:
    bdn::P<TodoModel> _model;
};
