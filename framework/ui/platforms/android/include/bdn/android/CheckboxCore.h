#pragma once

#include <bdn/Checkbox.h>
#include <bdn/CheckboxCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/CheckBox.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

namespace bdn::android
{

    class CheckboxCore : public ViewCore, virtual public bdn::CheckboxCore
    {
      public:
        CheckboxCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        TriState getState() const;

      public:
        void clicked() override;

      private:
        mutable wrapper::CheckBox _jCheckBox;
        TriState _state = TriState::off;
    };
}
