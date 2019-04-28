#pragma once

#include <bdn/Slider.h>

#import <Cocoa/Cocoa.h>
#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/util.hh>

@class BdnSliderHandler;

namespace bdn::mac
{
    class SliderCore : public ViewCore, virtual public bdn::Slider::Core
    {
      public:
        SliderCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        void init() override;

        void valueChanged();

      public:
        // Size sizeForSpace(Size availableSpace) const override;

      private:
        NSSlider *_nsSlider;
        BdnSliderHandler *_sliderHandler;
    };
}
