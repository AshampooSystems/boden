#pragma once

#include <bdn/UIUtil.h>
#include <bdn/View.h>

namespace bdn
{

    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Label)
    }

    class Label : public View
    {
      public:
        Property<String> text;
        Property<bool> wrap;

      public:
        Label(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            Property<String> text;
            Property<bool> wrap;

          public:
            virtual ~Core() = default;
        };
    };
}
