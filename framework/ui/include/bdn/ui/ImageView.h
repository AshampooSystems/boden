#pragma once

#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(ImageView)
    }

    class ImageView : public View
    {
      private:
        Property<Size> iOriginalSize;
        Property<float> iAspectRatio;

      public:
        Property<String> url;
        const Property<Size> originalSize = iOriginalSize;
        const Property<float> aspectRatio = iAspectRatio;

      public:
        ImageView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~ImageView() override = default;

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            Property<String> url;
            Property<Size> originalSize;
            Property<float> aspectRatio;
        };
    };
}
