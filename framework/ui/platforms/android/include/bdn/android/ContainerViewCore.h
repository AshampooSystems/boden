#pragma once

#include <bdn/ContainerView.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/IParentViewCore.h>
#include <bdn/android/JNativeViewGroup.h>

namespace bdn
{
    namespace android
    {

        class ContainerViewCore : public ViewCore, virtual public IParentViewCore
        {
          private:
            static std::shared_ptr<JNativeViewGroup> _createJNativeViewGroup(std::shared_ptr<ContainerView> outer);

          public:
            ContainerViewCore(std::shared_ptr<ContainerView> outer);

            Size calcPreferredSize(const Size &availableSpace) const override;

            void layout() override;

            Rect adjustAndSetBounds(const Rect &requestedBounds) override;

            double getUiScaleFactor() const override;

            void addChildJView(JView childJView) override;

            void removeChildJView(JView childJView) override;
        };
    }
}
