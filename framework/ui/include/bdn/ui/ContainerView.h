#pragma once

#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

#include <list>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(ContainerView)
    }

    class ContainerView : public View
    {
      public:
        ContainerView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        void addChildView(const std::shared_ptr<View> &childView);
        void removeChildView(const std::shared_ptr<View> &childView);
        void removeAllChildViews();

        std::vector<std::shared_ptr<View>> childViews() const override;

        std::optional<std::vector<std::shared_ptr<View>>::size_type> childIndex(const std::shared_ptr<View> &child)
        {
            const auto children = childViews();

            const auto it = std::find(children.begin(), children.end(), child);
            if (it == children.end()) {
                return std::nullopt;
            }

            return std::distance(children.begin(), it);
        }

      public:
        class Core
        {
          public:
            virtual ~Core() = default;

          public:
            virtual void addChildView(std::shared_ptr<View> child) = 0;
            virtual void removeChildView(std::shared_ptr<View> child) = 0;

            virtual std::vector<std::shared_ptr<View>> childViews() const = 0;
        };
    };
}
