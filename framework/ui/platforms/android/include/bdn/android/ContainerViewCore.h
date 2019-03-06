#pragma once

#include <bdn/ContainerView.h>
#include <bdn/android/IParentViewCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>

namespace bdn::android
{

    class ContainerViewCore : public ViewCore, virtual public IParentViewCore
    {
      private:
        static wrapper::View _createJNativeViewGroup(std::shared_ptr<ContainerView> outer);

      public:
        ContainerViewCore(std::shared_ptr<ContainerView> outer);
        ContainerViewCore(std::shared_ptr<ContainerView> outer, wrapper::View jView) : ViewCore(outer, jView) {}

        virtual ~ContainerViewCore();

        void dispose() override;

      public:
        double getUIScaleFactor() const override;

        void addChildCore(ViewCore *child) override;
        void removeChildCore(ViewCore *child) override;
    };
}
