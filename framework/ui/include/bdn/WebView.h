#pragma once

#include <bdn/View.h>

namespace bdn
{
    class WebView : public View
    {
      public:
        static constexpr char coreTypeName[] = "bdn.WebViewCore";

      public:
        WebView(std::shared_ptr<UIProvider> uiProvider = nullptr);
        virtual ~WebView() = default;

        Property<String> url;

        void loadURL(const String &url);

        String viewCoreTypeName() const override;
    };
}
