#pragma once

#include <bdn/String.h>
#include <bdn/View.h>

namespace bdn
{
    class StackCore
    {
      public:
        virtual void pushView(std::shared_ptr<View> view, String title) = 0;
        virtual void popView() = 0;
    };
}
