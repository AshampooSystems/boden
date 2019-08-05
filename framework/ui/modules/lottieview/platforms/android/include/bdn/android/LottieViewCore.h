#pragma once

#include <bdn/ui/lottie/View.h>

#include <bdn/android/ViewCore.h>

namespace bdn::ui::android
{
    class LottieViewCore : public ViewCore, virtual public lottie::View::Core
    {
      public:
        LottieViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~LottieViewCore() override = default;

        void init() override;

        void loadURL(const std::string &url) override;

        void onAnimationEnded();
    };
}
