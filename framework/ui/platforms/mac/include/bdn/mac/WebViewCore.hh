#pragma once

#include <bdn/ui/WebView.h>

#import <bdn/applecommon/WebViewNavigationController.hh>
#import <bdn/mac/ViewCore.hh>

@class WebViewUIDelegate;

namespace bdn::ui::mac
{
    class WebViewCore : public ViewCore, virtual public WebView::Core
    {
      public:
        WebViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        void init() override;
        void loadURL(const std::string &url) override;

      private:
        WebViewNavigationController *_navigationController;
        WebViewUIDelegate *_uiDelegate;
    };
}
