
#include <bdn/android/ButtonCore.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

namespace bdn::android
{
    ButtonCore::ButtonCore(std::shared_ptr<Button> outerButton)
        : ViewCore(outerButton, createAndroidViewClass<wrapper::Button>(outerButton)),
          _jButton(getJViewAS<wrapper::Button>())
    {
        _jButton.setSingleLine(true);

        setLabel(outerButton->label);

        wrapper::NativeViewCoreClickListener listener;
        _jButton.setOnClickListener(listener.cast<wrapper::OnClickListener>());
    }

    wrapper::Button &ButtonCore::getJButton() { return _jButton; }

    void ButtonCore::setLabel(const String &label)
    {
        _jButton.setText(label);
        _jButton.requestLayout();
    }

    void ButtonCore::clicked()
    {
        std::shared_ptr<View> view = outerView();
        if (view != nullptr) {
            ClickEvent evt(view);

            std::dynamic_pointer_cast<Button>(view)->onClick().notify(evt);
        }
    }

    double ButtonCore::getFontSizeDips() const
    {
        // the text size is in pixels
        return _jButton.getTextSize() / getUIScaleFactor();
    }
}
