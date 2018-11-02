#include <bdn/init.h>

#include "ViewModel.h"
#include "TodoModel.h"

ViewModel::ViewModel(TodoModel *model) : _model(model) {}
