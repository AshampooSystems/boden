#pragma once

#import <bdn/mac/ViewCore.hh>
#include <bdn/ui/ContainerView.h>

namespace bdn::ui::mac
{
    class ContainerViewCore : public ViewCore, virtual public ContainerView::Core
    {
      private:
        static NSView *_createContainer();

      public:
        ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        explicit ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, NSView *view);

      public:
        void init() override;

        void addChildView(std::shared_ptr<View> child) override;
        void removeChildView(std::shared_ptr<View> child) override;

        std::vector<std::shared_ptr<View>> childViews() const override;

      private:
        std::vector<std::shared_ptr<View>> _children;
    };
}
