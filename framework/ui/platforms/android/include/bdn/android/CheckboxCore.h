#pragma once

#include <bdn/ui/Checkbox.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/CheckBox.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

namespace bdn::ui::android
{

    class CheckboxCore : public ViewCore, virtual public Checkbox::Core
    {
      public:
        CheckboxCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        void clicked() override;

      private:
        mutable bdn::android::wrapper::CheckBox _jCheckBox;
    };
}
