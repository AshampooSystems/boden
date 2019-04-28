#pragma once

#include <bdn/Slider.h>
#include <bdn/log.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ViewCore.hh>

@interface BodenUISlider : UISlider <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

namespace bdn::ios
{
    class SliderCore : public ViewCore, virtual public bdn::Slider::Core
    {
      private:
        static BodenUISlider *createUISlider();

      public:
        SliderCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      private:
        UISlider *getUISlider();

      private:
        UISlider *_uiSlider;
    };
}
