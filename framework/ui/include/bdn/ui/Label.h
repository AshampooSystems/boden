#pragma once

#include <bdn/Text.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{

    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Label)
    }

    class Label : public View
    {
      public:
        Property<Text> text;
        Property<bool> wrap;

      public:
        Label(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      protected:
        void bindViewCore() override;
        void updateFromStylesheet() override;

      public:
        class Core
        {
          public:
            Property<Text> text;
            Property<bool> wrap;

          public:
            virtual ~Core() = default;
        };
    };
}
