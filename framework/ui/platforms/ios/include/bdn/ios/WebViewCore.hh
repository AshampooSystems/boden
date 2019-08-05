#pragma once

#import <bdn/ios/ViewCore.hh>

#include <bdn/ui/WebView.h>

#import <bdn/applecommon/WebViewNavigationController.hh>

namespace bdn::ui::ios
{
    class WebViewCore : public ViewCore, public WebView::Core
    {
      public:
        WebViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        void init() override;

      public:
        void loadURL(const std::string &url) override;

      private:
        WebViewNavigationController *_navigationController;
    };
}
