#pragma once

#include <bdn/android/ViewCore.h>
#include <bdn/android/JSwitch.h>
#include <bdn/ISwitchCore.h>
#include <bdn/Switch.h>
#include <bdn/Toggle.h>
#include <bdn/android/JNativeViewCoreClickListener.h>

namespace bdn
{
    namespace android
    {

        template <class T> class SwitchCore : public ViewCore, virtual public ISwitchCore
        {
          public:
            SwitchCore(std::shared_ptr<T> outer)
                : ViewCore(outer, ViewCore::createAndroidViewClass<JSwitch>(outer)), _jSwitch(getJViewAS<JSwitch>())
            {
                _jSwitch.setSingleLine(true);

                setLabel(outer->label);
                setOn(outer->on);

                bdn::android::JNativeViewCoreClickListener listener;
                _jSwitch.setOnClickListener(listener.cast<OnClickListener>());
            }

            JSwitch &getJSwitch() { return _jSwitch; }

            void setLabel(const String &label) override
            {
                _jSwitch.setText(label);

                // we must re-layout the button - otherwise its preferred size
                // is not updated.
                _jSwitch.requestLayout();
            }

            void setOn(const bool &on) override { _jSwitch.setChecked(on); }

            void clicked() override
            {
                std::shared_ptr<T> view = std::dynamic_pointer_cast<T>(getOuterViewIfStillAttached());
                if (view != nullptr) {
                    ClickEvent evt(view);

                    view->on = _jSwitch.isChecked();
                    view->onClick().notify(evt);
                }
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jSwitch.getTextSize() / getUiScaleFactor();
            }

          private:
            mutable JSwitch _jSwitch;
        };
    }
}
