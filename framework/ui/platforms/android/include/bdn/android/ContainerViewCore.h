#pragma once

#include <bdn/ContainerView.h>
#include <bdn/android/IParentViewCore.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/android/JView.h>
#include <bdn/android/ViewCore.h>

namespace bdn
{
    namespace android
    {

        class ContainerViewCore : public ViewCore, virtual public IParentViewCore
        {
          private:
            static JView _createJNativeViewGroup(std::shared_ptr<ContainerView> outer);

          public:
            ContainerViewCore(std::shared_ptr<ContainerView> outer);
            ContainerViewCore(std::shared_ptr<ContainerView> outer, JView jView) : ViewCore(outer, jView) {}

            virtual ~ContainerViewCore();

            void dispose() override;

          public:
            double getUIScaleFactor() const override;

            void addChildCore(ViewCore *child) override;
            void removeChildCore(ViewCore *child) override;
        };
    }
}
