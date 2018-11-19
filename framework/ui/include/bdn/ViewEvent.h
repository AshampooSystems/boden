#ifndef BDN_ViewEvent_H_
#define BDN_ViewEvent_H_

#include <bdn/View.h>

namespace bdn
{

    class ViewEvent : public Base
    {
      public:
        ViewEvent(View *view) { _view = view; }

        P<View> getView() { return _view; }

      protected:
        P<View> _view;
    };
}

#endif
