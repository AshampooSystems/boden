#pragma once

#include <bdn/android/wrapper/View.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kLottieAnimationViewClassName[] = "com/airbnb/lottie/LottieAnimationView";

    template <const char *javaClassName = kLottieAnimationViewClassName>
    class BaseLottieAnimationView : public BaseView<javaClassName>
    {
      public:
        using BaseView<javaClassName>::BaseView;

      public:
        JavaMethod<void(std::string)> setAnimationFromUrl{this, "setAnimationFromUrl"};
        JavaMethod<void()> playAnimation{this, "playAnimation"};
        JavaMethod<void()> pauseAnimation{this, "pauseAnimation"};
        JavaMethod<void(int)> setRepeatCount{this, "setRepeatCount"};
    };

    using LottieAnimationView = BaseLottieAnimationView<>;

    constexpr const char kNativeLottieAnimationViewClassName[] = "io/boden/android/NativeLottieAnimationView";
    class NativeLottieAnimationView : public BaseLottieAnimationView<kNativeLottieAnimationViewClassName>
    {
      public:
        using BaseLottieAnimationView<kNativeLottieAnimationViewClassName>::BaseLottieAnimationView;

      public:
        JavaMethod<void(std::string)> loadFromUrl{this, "loadFromUrl"};
    };
}
