
#include <bdn/android/ButtonCore.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

namespace bdn::detail
{
    CORE_REGISTER(Button, bdn::android::ButtonCore, Button)
}

namespace bdn::android
{
    ButtonCore::ButtonCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::Button>(viewCoreFactory)),
          _jButton(getJViewAS<wrapper::Button>())
    {
        _jButton.setSingleLine(true);

        label.onChange() += [=](auto va) {
            _jButton.setText(va->get());
            scheduleLayout();
        };

        wrapper::NativeViewCoreClickListener listener;
        _jButton.setOnClickListener(listener.cast<wrapper::OnClickListener>());
    }

    wrapper::Button &ButtonCore::getJButton() { return _jButton; }

    void ButtonCore::clicked() { _clickCallback.fire(); }
}
