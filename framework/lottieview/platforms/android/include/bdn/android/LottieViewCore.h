#pragma once

#include <bdn/LottieView.h>

#include <bdn/android/ViewCore.h>

namespace bdn::android
{
    class LottieViewCore : public ViewCore, virtual public bdn::LottieView::Core
    {
      public:
        LottieViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~LottieViewCore() override = default;

        std::shared_ptr<LottieViewCore> shared_from_this()
        {
            return std::dynamic_pointer_cast<LottieViewCore>(Base::shared_from_this());
        }

        void init() override;

        void loadURL(const String &url) override;

        void onAnimationEnded();
    };
}
