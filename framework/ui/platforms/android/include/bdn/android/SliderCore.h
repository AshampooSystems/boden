#pragma once

#include <bdn/Slider.h>

#include <bdn/android/ViewCore.h>

namespace bdn::android
{
    class SliderCore : public ViewCore, virtual public bdn::Slider::Core
    {
      public:
        SliderCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~SliderCore() override = default;

      public:
        void valueChanged(int newValue);
    };
}
