#pragma once

#include <bdn/View.h>

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
        static constexpr char coreTypeName[] = "bdn.WebViewCore";

      public:
        WebView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~WebView() override = default;

      public:
        void loadURL(const String &url);

        String viewCoreTypeName() const override;

      protected:
        void bindViewCore() override;
    };
}
