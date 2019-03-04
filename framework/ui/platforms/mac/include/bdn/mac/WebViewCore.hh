#pragma once

#include <bdn/WebView.h>

#import <bdn/mac/ChildViewCore.hh>

namespace bdn::mac
{
    class WebViewCore : public ChildViewCore
    {
      public:
        WebViewCore(std::shared_ptr<WebView> outer);
        virtual ~WebViewCore() = default;

        void loadURL(const String &url);
    };
}
