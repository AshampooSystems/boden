#pragma once

#include <bdn/LottieView.h>
#include <bdn/LottieViewCore.h>

#import <bdn/ios/ViewCore.hh>

@class LOTAnimationView;

namespace bdn::ios
{
    class LottieViewCore : public ViewCore, virtual public bdn::LottieViewCore
    {
      public:
        LottieViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        virtual ~LottieViewCore() = default;

        std::shared_ptr<LottieViewCore> shared_from_this()
        {
            return std::dynamic_pointer_cast<LottieViewCore>(Base::shared_from_this());
        }

        virtual void init() override;

        void loadURL(const String &url) override;

      private:
        void play();

      private:
        LOTAnimationView *animationView;
    };
}
