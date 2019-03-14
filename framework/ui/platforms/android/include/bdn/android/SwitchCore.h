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
        SwitchCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
            : ViewCore(uiProvider, createAndroidViewClass<wrapper::Switch>(uiProvider)),
              _jSwitch(getJViewAS<wrapper::Switch>())
        {
            _jSwitch.setSingleLine(true);

            label.onChange() += [=](auto va) {
                _jSwitch.setText(va->get());
                scheduleLayout();
            };

            on.onChange() += [=](auto va) { _jSwitch.setChecked(va->get()); };

            wrapper::NativeViewCoreClickListener listener;
            _jSwitch.setOnClickListener(listener.cast<wrapper::OnClickListener>());
        }

        wrapper::Switch &getJSwitch() { return _jSwitch; }

        void clicked() override
        {
            on = _jSwitch.isChecked();
            _clickCallback.fire();
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
