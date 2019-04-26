#include <bdn/android/LabelCore.h>

namespace bdn::detail
{
    CORE_REGISTER(Label, bdn::android::LabelCore, Label)
}

namespace bdn::android
{
    LabelCore::LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::TextView>(viewCoreFactory)),
          _jTextView(getJViewAS<wrapper::TextView>())
    {
        text.onChange() += [=](auto &property) {
            // Remove '\r' as android treats them as a space
            String textToSet = property.get();
            textToSet.erase(
                std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                textToSet.end());
            _jTextView.setText(textToSet);
            scheduleLayout();
        };

        wrap.onChange() += [=](auto &property) {
            _jTextView.setMaxLines(property.get() ? std::numeric_limits<int>::max() : 1);
            _wrap = property.get();
            scheduleLayout();
        };

        geometry.onChange() += [=](auto &property) {
            int widthPixels = property.get().width * getUIScaleFactor();

            if (_wrap) {
                _jTextView.setMaxWidth(widthPixels);
            }
        };
    }

    Size LabelCore::sizeForSpace(Size availableSpace) const
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
