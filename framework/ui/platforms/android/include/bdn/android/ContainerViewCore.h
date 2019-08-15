#pragma once

#include <bdn/ui/ContainerView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>

namespace bdn::ui::android
{

    class ContainerViewCore : public ViewCore, virtual public ContainerView::Core
    {
      private:
        static bdn::android::wrapper::View _createJNativeViewGroup(std::shared_ptr<ContainerView> outer);

      public:
        ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, bdn::android::wrapper::View jView)
            : ViewCore(viewCoreFactory, std::move(jView))
        {}

        ~ContainerViewCore() override;

      public:
        double getUIScaleFactor() const override;

        void addChildView(std::shared_ptr<View> child) override;
        void removeChildView(std::shared_ptr<View> child) override;
        std::vector<std::shared_ptr<View>> childViews() const override;

      private:
        std::vector<std::shared_ptr<View>> _children;

        // ViewCore interface
      public:
        void visitInternalChildren(const std::function<void(std::shared_ptr<View::Core>)> &function) override;
    };
}
