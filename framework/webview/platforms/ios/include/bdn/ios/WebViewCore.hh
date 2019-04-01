#pragma once

#import <bdn/ios/ViewCore.hh>

#include <bdn/WebView.h>

#import <bdn/applecommon/WebViewNavigationController.hh>

namespace bdn::ios
{
    class WebViewCore : public ViewCore, public bdn::WebView::Core
    {
      public:
        WebViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        void init() override;

      public:
        void loadURL(const String &url) override;

      private:
        WebViewNavigationController *_navigationController;
    };
}
