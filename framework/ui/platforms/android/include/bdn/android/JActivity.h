#pragma once

#include <bdn/android/JContext.h>
#include <bdn/android/JWindow.h>

namespace bdn::android
{
    constexpr const char kActivityClassName[] = "android/app/Activity";

    class JActivity : public JBaseContext<kActivityClassName>
    {
      public:
        using JBaseContext<kActivityClassName>::JBaseContext;

      public:
        java::Method<JWindow()> getWindow{this, "getWindow"};
    };
}
