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
        CheckboxCore(const ContextWrapper &ctxt)
            : ViewCore(createAndroidViewClass<wrapper::CheckBox>(ctxt)), _jCheckBox(getJViewAS<wrapper::CheckBox>())
        {
            _jCheckBox.setSingleLine(true);

            label.onChange() += [=](auto va) {
                _jCheckBox.setText(va->get());
                scheduleLayout();
            };

            state.onChange() += [=](auto va) {
                _jCheckBox.setChecked(va->get() == TriState::on);
                _state = state;
            };

            bdn::android::wrapper::NativeViewCoreClickListener listener;
            _jCheckBox.setOnClickListener(listener);
        }

        wrapper::CheckBox &getJCheckBox() { return _jCheckBox; }

        TriState getState() const { return _state; }

        void clicked() override { _clickCallback.fire(); }

      protected:
        double getFontSizeDips() const override
        {
            // the text size is in pixels
            return _jCheckBox.getTextSize() / getUIScaleFactor();
        }

      private:
        mutable wrapper::CheckBox _jCheckBox;
        TriState _state;
    };
}
