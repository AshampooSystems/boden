#pragma once

#include <bdn/LottieView.h>

#import <bdn/mac/ViewCore.hh>

@class LOTAnimationView;

namespace bdn::mac
{
    class LottieViewCore : public ViewCore, virtual public bdn::LottieView::Core
    {
      public:
        LottieViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~LottieViewCore() override;

      public:
        void init() override;
        void loadURL(const String &url) override;

      private:
        void updateGeometry();
        void play();

      private:
        LOTAnimationView *animationView;
    };
}
