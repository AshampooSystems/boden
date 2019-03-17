#pragma once

#include <bdn/Margin.h>
#include <bdn/String.h>
#include <bdn/property/Property.h>

namespace bdn
{
    class LottieViewCore
    {
      public:
        Property<bool> running;
        Property<bool> loop;

      public:
        LottieViewCore() = default;
        virtual ~LottieViewCore() = default;

      public:
        virtual void loadURL(const String &url) = 0;
    };
}
