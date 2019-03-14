#pragma once

#include <bdn/String.h>

namespace bdn
{
    class WebViewCore
    {
      public:
        Property<String> userAgent;

      public:
        Property<std::function<bool(WebView::RedirectRequest)>> redirectHandler;

      public:
        WebViewCore() = default;
        virtual ~WebViewCore() = default;

      public:
        virtual void loadURL(const String &url) = 0;
    };
}
