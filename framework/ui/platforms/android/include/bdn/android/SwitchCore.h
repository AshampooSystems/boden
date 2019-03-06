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
        SwitchCore(std::shared_ptr<Switch> outer)
            : ViewCore(outer, createAndroidViewClass<wrapper::Switch>(outer)), _jSwitch(getJViewAS<wrapper::Switch>())
        {
            _jSwitch.setSingleLine(true);

            setLabel(outer->label);
            setOn(outer->on);

            wrapper::NativeViewCoreClickListener listener;
            _jSwitch.setOnClickListener(listener.cast<wrapper::OnClickListener>());
        }

        wrapper::Switch &getJSwitch() { return _jSwitch; }

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
            std::shared_ptr<Switch> view = std::dynamic_pointer_cast<Switch>(outerView());
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
            return _jSwitch.getTextSize() / getUIScaleFactor();
        }

      private:
        mutable wrapper::Switch _jSwitch;
    };
}
