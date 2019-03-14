#pragma once

#include <bdn/UIUtil.h>
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
        WebView(std::shared_ptr<UIProvider> uiProvider = nullptr);
        virtual ~WebView() = default;

      public:
        void loadURL(const String &url);

        String viewCoreTypeName() const override;

      protected:
        virtual void bindViewCore() override;
    };
}
