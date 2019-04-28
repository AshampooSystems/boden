#pragma once

#include <bdn/View.h>
#include <bdn/ViewUtilities.h>

namespace bdn
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
