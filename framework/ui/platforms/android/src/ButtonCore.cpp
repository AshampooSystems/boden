
#include <bdn/android/ButtonCore.h>
#include <bdn/android/JNativeViewCoreClickListener.h>

namespace bdn
{
    namespace android
    {
        ButtonCore::ButtonCore(std::shared_ptr<Button> outerButton)
            : ViewCore(outerButton, createAndroidViewClass<JButton>(outerButton)), _jButton(getJViewAS<JButton>())
        {
            _jButton.setSingleLine(true);

            setLabel(outerButton->label);

            bdn::android::JNativeViewCoreClickListener listener;
            _jButton.setOnClickListener(listener.cast<OnClickListener>());
        }

        JButton &ButtonCore::getJButton() { return _jButton; }

        void ButtonCore::setLabel(const String &label)
        {
            _jButton.setText(label);
            _jButton.requestLayout();
        }

        void ButtonCore::clicked()
        {
            std::shared_ptr<View> view = getOuterViewIfStillAttached();
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
}
