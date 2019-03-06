#pragma once

#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/Window.h>

namespace bdn::android::wrapper
{
    constexpr const char kActivityClassName[] = "android/app/Activity";

    class Activity : public BaseContext<kActivityClassName>
    {
      public:
        using BaseContext<kActivityClassName>::BaseContext;

      public:
        JavaMethod<Window()> getWindow{this, "getWindow"};
    };
}
