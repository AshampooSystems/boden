#include <bdn/android/SwitchCore.h>

namespace bdn::detail
{
    CORE_REGISTER(Switch, bdn::android::SwitchCore, Switch)
}

namespace bdn::android
{

    SwitchCore::SwitchCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::Switch>(viewCoreFactory)),
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

    void SwitchCore::clicked()
    {
        on = _jSwitch.isChecked();
        _clickCallback.fire();
    }
}
