#pragma once

#include <bdn/ui/Slider.h>

#include <bdn/android/ViewCore.h>

namespace bdn::ui::android
{
    class SliderCore : public ViewCore, virtual public Slider::Core
    {
      public:
        SliderCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~SliderCore() override = default;

      public:
        void valueChanged(int newValue);
    };
}
