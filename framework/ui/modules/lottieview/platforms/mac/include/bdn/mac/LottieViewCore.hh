#pragma once

#include <bdn/ui/lottie/View.h>

#import <bdn/mac/ViewCore.hh>

@class LOTAnimationView;

namespace bdn::ui::mac
{
    class LottieViewCore : public ViewCore, virtual public bdn::ui::lottie::View::Core
    {
      public:
        LottieViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~LottieViewCore() override;

      public:
        void init() override;
        void loadURL(const std::string &url) override;

      private:
        void updateGeometry();
        void play();

      private:
        LOTAnimationView *animationView;
    };
}
