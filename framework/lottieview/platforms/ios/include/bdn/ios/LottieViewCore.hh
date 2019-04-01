#pragma once

#include <bdn/LottieView.h>

#import <bdn/ios/ViewCore.hh>

@class LOTAnimationView;

namespace bdn::ios
{
    class LottieViewCore : public ViewCore, virtual public bdn::LottieView::Core
    {
      public:
        LottieViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        void init() override;

        void loadURL(const String &url) override;

      private:
        void play();

      private:
        LOTAnimationView *animationView;
    };
}
