#pragma once

#include <bdn/WebView.h>
#include <bdn/WebViewCore.h>

#import <bdn/mac/ViewCore.hh>

namespace bdn::mac
{
    class WebViewCore : public ViewCore, virtual public bdn::WebViewCore
    {
      public:
        WebViewCore();
        virtual ~WebViewCore() = default;

        void loadURL(const String &url);
    };
}
