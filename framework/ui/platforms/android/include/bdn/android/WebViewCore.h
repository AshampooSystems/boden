#pragma once

#include <bdn/WebView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/WebView.h>

namespace bdn::android
{
    class WebViewCore : public ViewCore
    {
      public:
        WebViewCore(std::shared_ptr<WebView> outer);
        virtual ~WebViewCore() = default;

        void loadURL(const String &url);

      private:
        wrapper::WebView _jWebView;
    };
}
