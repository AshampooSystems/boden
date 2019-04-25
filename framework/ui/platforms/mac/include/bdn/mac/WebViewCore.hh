#pragma once

#include <bdn/WebView.h>

#import <bdn/applecommon/WebViewNavigationController.hh>
#import <bdn/mac/ViewCore.hh>

@class WebViewUIDelegate;

namespace bdn::mac
{
    class WebViewCore : public ViewCore, virtual public bdn::WebView::Core
    {
      public:
        WebViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        void init() override;
        void loadURL(const String &url) override;

      private:
        WebViewNavigationController *_navigationController;
        WebViewUIDelegate *_uiDelegate;
    };
}
