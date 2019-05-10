#pragma once

#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Slider)
    }

    class Slider : public View
    {
      public:
        Property<double> value;

      public:
        Slider(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            Property<double> value;

          public:
            virtual ~Core() = default;
        };
    };
}
