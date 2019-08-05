#pragma once

#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui::lottie
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(View)
    }

    class View : public ui::View
    {
      public:
        Property<std::string> url;

        Property<bool> running;
        Property<bool> loop;

      public:
        View(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~View() override = default;

      public:
        void loadURL(const std::string &url);

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            virtual ~Core() = default;

          public:
            Property<bool> running;
            Property<bool> loop;

          public:
            virtual void loadURL(const std::string &url) = 0;
        };
    };
}
