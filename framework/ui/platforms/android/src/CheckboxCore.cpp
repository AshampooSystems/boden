#include <bdn/android/CheckboxCore.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Checkbox, bdn::ui::android::CheckboxCore, Checkbox)
}

namespace bdn::ui::android
{

    CheckboxCore::CheckboxCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::CheckBox>(viewCoreFactory)),
          _jCheckBox(getJViewAS<bdn::android::wrapper::CheckBox>())
    {
        _jCheckBox.setSingleLine(true);

        label.onChange() += [=](auto &property) {
            _jCheckBox.setText(property.get());
            scheduleLayout();
        };

        state.onChange() += [=](auto &property) { _jCheckBox.setChecked(property.get() == TriState::On); };

        bdn::android::wrapper::NativeViewCoreClickListener listener;
        _jCheckBox.setOnClickListener(listener);
    }

    void CheckboxCore::clicked()
    {
        state = _jCheckBox.isChecked() ? TriState::On : TriState::Off;
        _clickCallback.fire();
    }
}
