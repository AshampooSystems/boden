#pragma once

#include <bdn/ui/Switch.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>
#include <bdn/android/wrapper/Switch.h>

namespace bdn::ui::android
{

    class SwitchCore : public ViewCore, virtual public Switch::Core
    {
      public:
        SwitchCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        void clicked() override;

      private:
        mutable bdn::android::wrapper::Switch _jSwitch;
    };
}
