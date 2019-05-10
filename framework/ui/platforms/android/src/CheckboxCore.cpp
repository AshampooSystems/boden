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

        state.onChange() += [=](auto &property) {
            _jCheckBox.setChecked(property.get() == TriState::on);
            _state = state;
        };

        bdn::android::wrapper::NativeViewCoreClickListener listener;
        _jCheckBox.setOnClickListener(listener);
    }

    TriState CheckboxCore::getState() const { return _state; }

    void CheckboxCore::clicked() { _clickCallback.fire(); }
}
