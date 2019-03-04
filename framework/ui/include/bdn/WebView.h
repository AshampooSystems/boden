#pragma once

#include <bdn/View.h>

namespace bdn
{
    class WebView : public View
    {
      public:
        static constexpr char coreTypeName[] = "bdn.WebViewCore";

      public:
        WebView();
        virtual ~WebView() = default;

        Property<String> url;

        void loadURL(const String &url);

        String viewCoreTypeName() const override;
    };
}
