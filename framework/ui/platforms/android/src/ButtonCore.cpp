
#include <bdn/android/ButtonCore.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Button, bdn::ui::android::ButtonCore, Button)
}

namespace bdn::ui::android
{
    ButtonCore::ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::Button>(viewCoreFactory)),
          _jButton(getJViewAS<bdn::android::wrapper::Button>())
    {
        _jButton.setSingleLine(true);

        label.onChange() += [=](auto &property) {
            _jButton.setText(property.get());
            scheduleLayout();
        };

        bdn::android::wrapper::NativeViewCoreClickListener listener;
        _jButton.setOnClickListener(listener.cast<bdn::android::wrapper::OnClickListener>());
    }

    bdn::android::wrapper::Button &ButtonCore::getJButton() { return _jButton; }

    void ButtonCore::clicked() { _clickCallback.fire(); }
}
