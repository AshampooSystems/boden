#include <bdn/android/TextViewCore.h>

namespace bdn::detail
{
    CORE_REGISTER(TextView, bdn::android::TextViewCore, TextView)
}

namespace bdn::android
{
    TextViewCore::TextViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::TextView>(viewCoreFactory)),
          _jTextView(getJViewAS<wrapper::TextView>())
    {
        text.onChange() += [=](auto va) {
            // Remove '\r' as android treats them as a space
            String textToSet = va->get();
            textToSet.erase(
                std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                textToSet.end());
            _jTextView.setText(textToSet);
            scheduleLayout();
        };

        wrap.onChange() += [=](auto va) {
            _jTextView.setMaxLines(va->get() ? std::numeric_limits<int>::max() : 1);
            _wrap = va->get();
            scheduleLayout();
        };

        geometry.onChange() += [=](auto va) {
            int widthPixels = va->get().width * getUIScaleFactor();

            if (_wrap) {
                _jTextView.setMaxWidth(widthPixels);
            }
        };
    }

    Size TextViewCore::sizeForSpace(Size availableSpace) const
    {
        if (_wrap) {
            _jTextView.setMaxWidth((int)(availableSpace.width * getUIScaleFactor()));
        }

        Size result = ViewCore::sizeForSpace(availableSpace);

        if (_wrap) {
            _jTextView.setMaxWidth((int)(geometry->width * getUIScaleFactor()));
        }

        return result;
    }
}
