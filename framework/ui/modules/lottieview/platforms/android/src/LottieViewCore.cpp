#include <bdn/android/LottieViewCore.h>
#include <bdn/android/wrapper/LottieAnimationView.h>
#include <bdn/ui/lottie/View.h>

#include <bdn/log.h>

#include "../include/bdn/android/wrapper/LottieAnimationView.h"
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui::lottie::detail
{
    CORE_REGISTER(View, bdn::ui::android::LottieViewCore, View)
}

namespace bdn::ui::android
{
    LottieViewCore::LottieViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory,
                   createAndroidViewClass<bdn::android::wrapper::NativeLottieAnimationView>(viewCoreFactory))
    {}

    void LottieViewCore::init()
    {
        ViewCore::init();

        running.onChange() += [=](auto &property) {
            auto animationView = getJViewAS<bdn::android::wrapper::LottieAnimationView>();
            if (property.get()) {
                animationView.playAnimation();
            } else {
                animationView.pauseAnimation();
            }
        };
        loop.onChange() += [=](auto &property) {
            auto animationView = getJViewAS<bdn::android::wrapper::LottieAnimationView>();
            if (property.get()) {
                animationView.setRepeatCount(0xffffffff);
            } else {
                animationView.setRepeatCount(0);
            }
        };
    }

    void LottieViewCore::loadURL(const std::string &url)
    {
        auto animationView = getJViewAS<bdn::android::wrapper::NativeLottieAnimationView>();
        animationView.loadFromUrl(url);
    }

    void LottieViewCore::onAnimationEnded() { running = false; }
}
