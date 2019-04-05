#pragma once

#include <bdn/View.h>
#include <bdn/ViewUtilities.h>

namespace bdn
{
    namespace webview::detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(WebView)
    }

    class WebView : public View
    {
      public:
        class RedirectRequest
        {
          public:
            String url;
        };

      public:
        Property<String> url;
        Property<String> userAgent;
        Property<std::function<bool(RedirectRequest)>> redirectHandler;

      public:
        WebView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~WebView() override = default;

      public:
        void loadURL(const String &url);

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            Property<String> userAgent;

          public:
            Property<std::function<bool(WebView::RedirectRequest)>> redirectHandler;

          public:
            virtual void loadURL(const String &url) = 0;
        };
    };
}
