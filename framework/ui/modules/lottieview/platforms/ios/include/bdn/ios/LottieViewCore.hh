#pragma once

#include <bdn/ui/lottie/View.h>

#import <bdn/ios/ViewCore.hh>

@class LOTAnimationView;

namespace bdn::ui::ios
{
    class LottieViewCore : public ViewCore, virtual public lottie::View::Core
    {
      public:
        LottieViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~LottieViewCore() override;

      public:
        void init() override;

        void loadURL(const std::string &url) override;

      private:
        void play();

      private:
        LOTAnimationView *animationView;
    };
}
