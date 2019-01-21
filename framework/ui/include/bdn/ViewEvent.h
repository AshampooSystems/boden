#pragma once

#include <bdn/View.h>

namespace bdn
{

    class ViewEvent : public Base
    {
      public:
        ViewEvent(std::shared_ptr<View> view) { _view = view; }

        std::shared_ptr<View> getView() { return _view; }

      protected:
        std::shared_ptr<View> _view;
    };
}
