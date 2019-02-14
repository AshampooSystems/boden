#pragma once

#include <bdn/View.h>
#include <bdn/String.h>

namespace bdn
{
    class StackCore
    {
      public:
        virtual void pushView(std::shared_ptr<View> view, String title) = 0;
        virtual void popView() = 0;
    };
}
