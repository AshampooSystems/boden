
#include <bdn/android/AttributedString.h>
#include <bdn/android/ButtonCore.h>
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Button, bdn::ui::android::ButtonCore, Button)
}

namespace bdn::ui::android
{
    ButtonCore::ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeButton>(viewCoreFactory)),
          _jButton(getJViewAS<bdn::android::wrapper::NativeButton>())
    {
        label.onChange() += [=](auto &property) {
            textChanged(property.get());
            scheduleLayout();
        };

        imageURL.onChange() += [=](auto &property) { imageChanged(property.get()); };

        bdn::android::wrapper::NativeViewCoreClickListener listener;
        _jButton.setOnClickListener(listener.cast<bdn::android::wrapper::OnClickListener>());
    }

    bdn::android::wrapper::NativeButton &ButtonCore::getJButton() { return _jButton; }

    void ButtonCore::clicked() { _clickCallback.fire(); }

    void ButtonCore::textChanged(const Text &text)
    {
        std::visit(
            [&jView = this->_jButton](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, std::string>) {
                    // Remove '\r' as android treats them as a space
                    std::string textToSet = arg;
                    textToSet.erase(
                        std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                        textToSet.end());
                    jView.setText(textToSet);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto attrString = std::dynamic_pointer_cast<bdn::android::AttributedString>(arg)) {
                        jView.setText(attrString->spanned());
                    }
                }
            },
            text);

        markDirty();
        scheduleLayout();
    }

    void ButtonCore::imageChanged(const std::string &url) { _jButton.setImage(url); }
}
