#pragma once

#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(WebView)
    }

    class WebView : public View
    {
      public:
        class RedirectRequest
        {
          public:
            std::string url;
        };

      public:
        Property<std::string> url;
        Property<std::string> userAgent;
        Property<std::function<bool(RedirectRequest)>> redirectHandler;

      public:
        WebView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~WebView() override = default;

      public:
        void loadURL(const std::string &url);

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            Property<std::string> userAgent;

          public:
            Property<std::function<bool(WebView::RedirectRequest)>> redirectHandler;

          public:
            virtual void loadURL(const std::string &url) = 0;
        };
    };
}
