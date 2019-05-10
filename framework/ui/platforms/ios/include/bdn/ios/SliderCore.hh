#pragma once

#include <bdn/log.h>
#include <bdn/ui/Slider.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ViewCore.hh>

@interface BodenUISlider : UISlider <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
@end

namespace bdn::ui::ios
{
    class SliderCore : public ViewCore, virtual public Slider::Core
    {
      private:
        static BodenUISlider *createUISlider();

      public:
        SliderCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      private:
        UISlider *getUISlider();

      private:
        UISlider *_uiSlider;
    };
}
