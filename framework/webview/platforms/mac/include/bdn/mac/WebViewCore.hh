#pragma once

#include <bdn/WebView.h>
#include <bdn/WebViewCore.h>

#import <bdn/applecommon/WebViewNavigationController.hh>
#import <bdn/mac/ViewCore.hh>

namespace bdn::mac
{
    class WebViewCore : public ViewCore, virtual public bdn::WebViewCore
    {
      public:
        WebViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~WebViewCore() override = default;

        std::shared_ptr<WebViewCore> shared_from_this()
        {
            return std::dynamic_pointer_cast<WebViewCore>(Base::shared_from_this());
        }

        void init() override;

        void loadURL(const String &url) override;

      private:
        WebViewNavigationController *_navigationController;
    };
}
