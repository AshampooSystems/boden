#pragma once

#include <bdn/ContainerView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>

namespace bdn::android
{

    class ContainerViewCore : public ViewCore, virtual public bdn::ContainerView::Core
    {
      private:
        static wrapper::View _createJNativeViewGroup(std::shared_ptr<ContainerView> outer);

      public:
        ContainerViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ContainerViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory, wrapper::View jView)
            : ViewCore(viewCoreFactory, std::move(jView))
        {}

        ~ContainerViewCore() override;

      public:
        double getUIScaleFactor() const override;

        void addChildView(std::shared_ptr<View> child) override;
        void removeChildView(std::shared_ptr<View> child) override;
        std::list<std::shared_ptr<View>> childViews() override;

      private:
        std::list<std::shared_ptr<View>> _children;

        // ViewCore interface
      public:
        void visitInternalChildren(const std::function<void(std::shared_ptr<bdn::View::Core>)> &function) override;
    };
}
