#include <bdn/android/LabelCore.h>

#include <bdn/android/AttributedString.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Label, bdn::ui::android::LabelCore, Label)
}

namespace bdn::ui::android
{
    LabelCore::LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::AppCompatTextView>(viewCoreFactory)),
          _jTextView(getJViewAS<bdn::android::wrapper::TextView>())
    {
        text.onChange() += [=](auto &property) { textChanged(property.get()); };

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

    void LabelCore::textChanged(const Text &text)
    {
        std::visit(
            [&jTextView = this->_jTextView](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, String>) {
                    // Remove '\r' as android treats them as a space
                    String textToSet = arg;
                    textToSet.erase(
                        std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                        textToSet.end());
                    jTextView.setText(textToSet);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto attrString = std::dynamic_pointer_cast<bdn::android::AttributedString>(arg)) {
                        jTextView.setText(attrString->spanned());
                    }
                }
            },
            text);

        scheduleLayout();
    }
}
