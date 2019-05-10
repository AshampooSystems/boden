#pragma once

#include <bdn/ui/WebView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/WebView.h>

namespace bdn::ui::android
{
    class WebViewCore : public ViewCore, virtual public WebView::Core
    {
      public:
        WebViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~WebViewCore() override = default;

        void loadURL(const String &url) override;

      private:
        bdn::android::wrapper::WebView _jWebView;
    };
}
