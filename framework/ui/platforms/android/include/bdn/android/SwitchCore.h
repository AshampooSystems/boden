#pragma once

#include <bdn/Switch.h>
#include <bdn/SwitchCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>
#include <bdn/android/wrapper/Switch.h>

namespace bdn::android
{

    class SwitchCore : public ViewCore, virtual public bdn::SwitchCore
    {
      public:
        SwitchCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        void clicked() override;

      private:
        mutable wrapper::Switch _jSwitch;
    };
}
