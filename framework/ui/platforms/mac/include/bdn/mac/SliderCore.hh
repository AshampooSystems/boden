#pragma once

#include <bdn/ui/Slider.h>

#import <Cocoa/Cocoa.h>
#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/util.hh>

@class BdnSliderHandler;

namespace bdn::ui::mac
{
    class SliderCore : public ViewCore, virtual public Slider::Core
    {
      public:
        SliderCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        void init() override;

        void valueChanged();

      public:
        // Size sizeForSpace(Size availableSpace) const override;

      private:
        NSSlider *_nsSlider;
        BdnSliderHandler *_sliderHandler;
    };
}
