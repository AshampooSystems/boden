
#include <bdn/android/ButtonCore.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

namespace bdn::android
{
    ButtonCore::ButtonCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createAndroidViewClass<wrapper::Button>(uiProvider)),
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

    double ButtonCore::getFontSizeDips() const
    {
        // the text size is in pixels
        return _jButton.getTextSize() / getUIScaleFactor();
    }
}
