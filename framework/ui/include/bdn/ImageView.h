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
        virtual ~ImageView() = default;

      public:
        static constexpr char coreTypeName[] = "bdn.ImageViewCore";
        virtual String viewCoreTypeName() const override;

      protected:
        virtual void bindViewCore() override;
    };
}
