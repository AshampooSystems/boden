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
        WebViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        virtual ~WebViewCore() = default;

        std::shared_ptr<WebViewCore> shared_from_this()
        {
            return std::dynamic_pointer_cast<WebViewCore>(Base::shared_from_this());
        }

        virtual void init() override;

        void loadURL(const String &url) override;

      private:
        WebViewNavigationController *_navigationController;
    };
}
