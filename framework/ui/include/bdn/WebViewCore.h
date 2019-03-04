#pragma once

#include <bdn/String.h>

namespace bdn
{
    class WebViewCore
    {
      public:
        virtual void loadURL(const String &url) = 0;
    };
}
