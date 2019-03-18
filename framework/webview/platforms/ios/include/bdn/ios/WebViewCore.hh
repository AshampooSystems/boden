#pragma once

#import <bdn/ios/ViewCore.hh>

#include <bdn/WebView.h>
#include <bdn/WebViewCore.h>

#import <bdn/applecommon/WebViewNavigationController.hh>

namespace bdn::ios
{
    class WebViewCore : public ViewCore, virtual public bdn::WebViewCore
    {
      public:
        WebViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ~WebViewCore() override = default;

        void init() override;

      public:
        void loadURL(const String &url) override;

      private:
        WebViewNavigationController *_navigationController;
    };
}
