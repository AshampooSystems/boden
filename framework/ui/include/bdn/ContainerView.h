#pragma once

#include <bdn/UIUtil.h>
#include <bdn/View.h>

#include <list>

namespace bdn
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
        void removeAllChildViews() override;
        std::list<std::shared_ptr<View>> childViews() override;

      public:
        void childViewStolen(const std::shared_ptr<View> &childView) override;

      public:
        class Core
        {
          public:
            virtual ~Core() = default;

          public:
            virtual void addChildView(std::shared_ptr<View> child) = 0;
            virtual void removeChildView(std::shared_ptr<View> child) = 0;

            virtual std::list<std::shared_ptr<View>> childViews() = 0;
        };
    };
}
