#pragma once

#import <bdn/ios/ViewCore.hh>

#include <bdn/WebView.h>
#include <bdn/WebViewCore.h>

namespace bdn::ios
{
    class WebViewCore : public ViewCore, virtual public bdn::WebViewCore
    {
      public:
        WebViewCore();
        virtual ~WebViewCore() = default;

        void loadURL(const String &url);
    };
}
