#pragma once

#include <bdn/View.h>
#include <bdn/ViewUtilities.h>

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

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            Property<String> url;
        };
    };
}
