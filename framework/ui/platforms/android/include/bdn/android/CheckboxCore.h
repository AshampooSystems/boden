#pragma once

#include <bdn/Checkbox.h>
#include <bdn/CheckboxCore.h>
#include <bdn/android/JCheckBox.h>
#include <bdn/android/JNativeViewCoreClickListener.h>
#include <bdn/android/ViewCore.h>

namespace bdn
{
    namespace android
    {

        class CheckboxCore : public ViewCore, virtual public bdn::CheckboxCore
        {
          public:
            CheckboxCore(std::shared_ptr<Checkbox> outer)
                : ViewCore(outer, ViewCore::createAndroidViewClass<JCheckBox>(outer)),
                  _jCheckBox(getJViewAS<JCheckBox>())
            {
                _jCheckBox.setSingleLine(true);

                setLabel(outer->label);
                setState(outer->state);

                bdn::android::JNativeViewCoreClickListener listener;
                _jCheckBox.setOnClickListener(listener);
            }

            JCheckBox &getJCheckBox() { return _jCheckBox; }

            void setLabel(const String &label) override
            {
                _jCheckBox.setText(label);
                _jCheckBox.requestLayout();
            }

            void setState(const TriState &state) override
            {
                _jCheckBox.setChecked(state == TriState::on);
                _state = state;
            }

            TriState getState() const { return _state; }

            void clicked() override
            {
                std::shared_ptr<Checkbox> view = std::dynamic_pointer_cast<Checkbox>(getOuterViewIfStillAttached());
                if (view != nullptr) {
                    ClickEvent evt(view);

                    std::shared_ptr<Checkbox> checkbox = std::dynamic_pointer_cast<Checkbox>(view);

                    _state = _jCheckBox.isChecked() ? TriState::on : TriState::off;

                    // User interaction cannot set the checkbox into mixed state
                    if (checkbox)
                        checkbox->state = (_state);

                    view->onClick().notify(evt);
                }
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jCheckBox.getTextSize() / getUIScaleFactor();
            }

          private:
            mutable JCheckBox _jCheckBox;
            TriState _state;
        };
    }
}
