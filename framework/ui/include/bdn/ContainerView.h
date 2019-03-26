#pragma once

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
        static constexpr char coreTypeName[] = "bdn.ContainerViewCore";
        String viewCoreTypeName() const override { return coreTypeName; }

      public:
        void addChildView(const std::shared_ptr<View> &childView);
        void removeChildView(const std::shared_ptr<View> &childView);
        void removeAllChildViews() override;
        std::list<std::shared_ptr<View>> childViews() override;

      public:
        void childViewStolen(const std::shared_ptr<View> &childView) override;
    };
}
