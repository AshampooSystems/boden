#include <bdn/LottieView.h>
#include <bdn/android/LottieViewCore.h>
#include <bdn/android/wrapper/LottieAnimationView.h>

#include <bdn/log.h>

#include "../include/bdn/android/wrapper/LottieAnimationView.h"
#include <bdn/UIUtil.h>

namespace bdn::lottieview::detail
{
    CORE_REGISTER(LottieView, bdn::android::LottieViewCore, LottieView)
}

namespace bdn::android
{
    LottieViewCore::LottieViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createAndroidViewClass<wrapper::NativeLottieAnimationView>(uiProvider))
    {}

    void LottieViewCore::init()
    {
        ViewCore::init();

        running.onChange() += [=](auto va) {
            auto animationView = getJViewAS<wrapper::LottieAnimationView>();
            if (va->get()) {
                animationView.playAnimation();
            } else {
                animationView.pauseAnimation();
            }
        };
        loop.onChange() += [=](auto va) {
            auto animationView = getJViewAS<wrapper::LottieAnimationView>();
            if (va->get()) {
                animationView.setRepeatCount(0xffffffff);
            } else {
                animationView.setRepeatCount(0);
            }
        };
    }

    void LottieViewCore::loadURL(const String &url)
    {
        auto animationView = getJViewAS<wrapper::LottieAnimationView>();
        animationView.setAnimationFromUrl(url);
    }

    void LottieViewCore::onAnimationEnded() { running = false; }
}
