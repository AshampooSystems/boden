#pragma once

#import <bdn/ios/ViewCore.hh>

#include <bdn/WebView.h>
#include <bdn/WebViewCore.h>

namespace bdn
{
    namespace ios
    {
        class WebViewCore : public ViewCore, virtual public bdn::WebViewCore
        {
          public:
            WebViewCore(std::shared_ptr<WebView> outer);
            virtual ~WebViewCore() = default;

            void loadURL(const String &url);
        };
    }
}
