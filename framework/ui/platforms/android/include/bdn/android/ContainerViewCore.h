#pragma once

#include <bdn/ContainerView.h>
#include <bdn/ContainerViewCore.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>

namespace bdn::android
{

    class ContainerViewCore : public ViewCore, virtual public bdn::ContainerViewCore
    {
      private:
        static wrapper::View _createJNativeViewGroup(std::shared_ptr<ContainerView> outer);

      public:
        ContainerViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ContainerViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, wrapper::View jView)
            : ViewCore(uiProvider, jView)
        {}

        virtual ~ContainerViewCore();

      public:
        double getUIScaleFactor() const override;

        virtual void addChildView(std::shared_ptr<View> child) override;
        virtual void removeChildView(std::shared_ptr<View> child) override;
        virtual std::list<std::shared_ptr<View>> childViews() override;

      private:
        std::list<std::shared_ptr<View>> _children;

        // ViewCore interface
      public:
        virtual void visitInternalChildren(std::function<void(std::shared_ptr<bdn::ViewCore>)> function) override;
    };
}
