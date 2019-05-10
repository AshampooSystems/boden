#include <bdn/android/SwitchCore.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Switch, bdn::ui::android::SwitchCore, Switch)
}

namespace bdn::ui::android
{

    SwitchCore::SwitchCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::Switch>(viewCoreFactory)),
          _jSwitch(getJViewAS<bdn::android::wrapper::Switch>())
    {
        _jSwitch.setSingleLine(true);

        label.onChange() += [=](auto &property) {
            _jSwitch.setText(property.get());
            scheduleLayout();
        };

        on.onChange() += [=](auto &property) { _jSwitch.setChecked(property.get()); };

        bdn::android::wrapper::NativeViewCoreClickListener listener;
        _jSwitch.setOnClickListener(listener.cast<bdn::android::wrapper::OnClickListener>());
    }

    void SwitchCore::clicked()
    {
        on = _jSwitch.isChecked();
        _clickCallback.fire();
    }
}
