#pragma once

#include <bdn/ui/View.h>
#include <utility>

namespace bdn::ui
{
    class ViewEvent
    {
      public:
        ViewEvent(std::shared_ptr<View> view) { _view = std::move(view); }

        std::shared_ptr<View> getView() { return _view; }

      protected:
        std::shared_ptr<View> _view;
    };
}
