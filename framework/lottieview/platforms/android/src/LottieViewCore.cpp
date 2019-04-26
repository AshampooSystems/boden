#include <bdn/LottieView.h>
#include <bdn/android/LottieViewCore.h>
#include <bdn/android/wrapper/LottieAnimationView.h>

#include <bdn/log.h>

#include "../include/bdn/android/wrapper/LottieAnimationView.h"
#include <bdn/ViewUtilities.h>

namespace bdn::lottieview::detail
{
    CORE_REGISTER(LottieView, bdn::android::LottieViewCore, LottieView)
}

namespace bdn::android
{
    LottieViewCore::LottieViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::NativeLottieAnimationView>(viewCoreFactory))
    {}

    void LottieViewCore::init()
    {
        ViewCore::init();

        running.onChange() += [=](auto &property) {
            auto animationView = getJViewAS<wrapper::LottieAnimationView>();
            if (property.get()) {
                animationView.playAnimation();
            } else {
                animationView.pauseAnimation();
            }
        };
        loop.onChange() += [=](auto &property) {
            auto animationView = getJViewAS<wrapper::LottieAnimationView>();
            if (property.get()) {
                animationView.setRepeatCount(0xffffffff);
            } else {
                animationView.setRepeatCount(0);
            }
        };
    }

    void LottieViewCore::loadURL(const String &url)
    {
        auto animationView = getJViewAS<wrapper::NativeLottieAnimationView>();
        animationView.loadFromUrl(url);
    }

    void LottieViewCore::onAnimationEnded() { running = false; }
}
