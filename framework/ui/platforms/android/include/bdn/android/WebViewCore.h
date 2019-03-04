#pragma once

#include <bdn/WebView.h>

#include <bdn/android/JWebView.h>
#include <bdn/android/ViewCore.h>

namespace bdn::android
{
    class WebViewCore : public ViewCore
    {
      public:
        WebViewCore(std::shared_ptr<WebView> outer);
        virtual ~WebViewCore() = default;

        void loadURL(const String &url);

      private:
        JWebView _jWebView;
    };
}
