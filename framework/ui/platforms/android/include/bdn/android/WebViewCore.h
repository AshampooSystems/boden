#pragma once

#include <bdn/WebView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/WebView.h>

namespace bdn::android
{
    class WebViewCore : public ViewCore, virtual public bdn::WebView::Core
    {
      public:
        WebViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~WebViewCore() override = default;

        void loadURL(const String &url) override;

      private:
        wrapper::WebView _jWebView;
    };
}
