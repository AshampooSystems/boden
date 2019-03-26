#pragma once

#include <bdn/View.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(ImageView)
    }

    class ImageView : public View
    {
      public:
        Property<String> url;

      public:
        ImageView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~ImageView() override = default;

      public:
        static constexpr char coreTypeName[] = "bdn.ImageViewCore";
        String viewCoreTypeName() const override;

      protected:
        void bindViewCore() override;
    };
}
