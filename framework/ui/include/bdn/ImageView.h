#pragma once

#include <bdn/View.h>

namespace bdn
{
    class ImageView : public View
    {
      public:
        Property<String> url;

      public:
        ImageView(std::shared_ptr<UIProvider> uiProvider = nullptr);
        ~ImageView() override = default;

      public:
        static constexpr char coreTypeName[] = "bdn.ImageViewCore";
        String viewCoreTypeName() const override;

      protected:
        void bindViewCore() override;
    };
}
