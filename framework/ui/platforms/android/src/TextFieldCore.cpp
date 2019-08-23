#include <bdn/android/AttributedString.h>
#include <bdn/android/TextFieldCore.h>
#include <bdn/android/wrapper/EditorInfo.h>
#include <bdn/android/wrapper/InputType.h>

using bdn::android::wrapper::Context;
using bdn::android::wrapper::EditorInfo;
using bdn::android::wrapper::InputType;
using bdn::android::wrapper::OnEditorActionListener;
using bdn::android::wrapper::TextWatcher;

namespace bdn::ui::detail
{
    CORE_REGISTER(TextField, bdn::ui::android::TextFieldCore, TextField)
}

namespace bdn::ui::android
{
    TextFieldCore::TextFieldCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeEditText>(viewCoreFactory)),
          _jEditText(getJViewAS<bdn::android::wrapper::NativeEditText>()),
          _watcher(_jEditText.cast<bdn::android::wrapper::TextView>())
    {
        _jEditText.setSingleLine(true);

        _jEditText.addTextChangedListener(_watcher.cast<TextWatcher>());

        _jEditText.setOnEditorActionListener(_onEditorActionListener.cast<OnEditorActionListener>());

        text.onChange() += [=](auto &property) {
            _jEditText.removeTextChangedListener(_watcher.cast<TextWatcher>());
            std::string currentText = _jEditText.getText();
            if (property.get() != currentText) {
                _jEditText.setText(property.get());
            }
            _jEditText.addTextChangedListener(_watcher.cast<TextWatcher>());
        };

        font.onChange() += [=](auto &property) { setFont(property.get()); };

        autocorrectionType.onChange() += [=](auto &property) { updateInputType(); };

        returnKeyType.onChange() += [=](auto &property) { setReturnKeyType(property.get()); };

        placeholder.onChange() += [this](auto &property) { setPlaceholder(property.get()); };

        textInputType.onChange() += [this](auto &property) { updateInputType(); };

        obscureInput.onChange() += [this](auto &property) { updateInputType(); };
    }

    void TextFieldCore::focus()
    {
        _jEditText.requestFocus();

        bdn::android::wrapper::InputMethodManager inputManager(
            _jEditText.getContext().getSystemService(bdn::android::wrapper::Context::INPUT_METHOD_SERVICE).getRef_());
        inputManager.showSoftInput(_jEditText.cast<bdn::android::wrapper::View>(), 0);
    }

    void TextFieldCore::beforeTextChanged(const std::string &string, int start, int count, int after) {}

    void TextFieldCore::onTextChanged(const std::string &string, int start, int before, int count) {}

    void TextFieldCore::afterTextChanged()
    {
        std::string newText = _jEditText.getText();
        text = newText;
    }

    bool TextFieldCore::onEditorAction(int actionId, const bdn::android::wrapper::KeyEvent &keyEvent)
    {
        bdn::android::wrapper::InputMethodManager inputManager(
            _jEditText.getContext().getSystemService(bdn::android::wrapper::Context::INPUT_METHOD_SERVICE).getRef_());
        inputManager.hideSoftInputFromWindow(_jEditText.getWindowToken(), 0);

        submitCallback.fire();

        return true;
    }

    void TextFieldCore::setFont(const Font &font)
    {
        _jEditText.setFont(font.family, (int)font.size.type, font.size.value, font.weight,
                           font.style == Font::Style::Italic);
    }

    void TextFieldCore::setReturnKeyType(const ReturnKeyType returnKeyType)
    {
        const int currentIMEOptions = _jEditText.getImeOptions();
        int imeOption = (int)EditorInfo::IME_ACTION_UNSPECIFIED;

        switch (returnKeyType) {
        case ReturnKeyType::Default:
            imeOption = (int)EditorInfo::IME_ACTION_UNSPECIFIED;
            break;
        case ReturnKeyType::Continue:
            imeOption = (int)EditorInfo::IME_ACTION_UNSPECIFIED;
            break;
        case ReturnKeyType::Done:
            imeOption = (int)EditorInfo::IME_ACTION_DONE;
            break;
        case ReturnKeyType::EmergencyCall:
            imeOption = (int)EditorInfo::IME_ACTION_UNSPECIFIED;
            break;
        case ReturnKeyType::Go:
            imeOption = (int)EditorInfo::IME_ACTION_GO;
            break;
        case ReturnKeyType::Join:
            imeOption = (int)EditorInfo::IME_ACTION_UNSPECIFIED;
            break;
        case ReturnKeyType::Next:
            imeOption = (int)EditorInfo::IME_ACTION_NEXT;
            break;
        case ReturnKeyType::None:
            imeOption = (int)EditorInfo::IME_ACTION_NONE;
            break;
        case ReturnKeyType::Previous:
            imeOption = (int)EditorInfo::IME_ACTION_UNSPECIFIED;
            break;
        case ReturnKeyType::Route:
            imeOption = (int)EditorInfo::IME_ACTION_UNSPECIFIED;
            break;
        case ReturnKeyType::Search:
            imeOption = (int)EditorInfo::IME_ACTION_SEARCH;
            break;
        case ReturnKeyType::Send:
            imeOption = (int)EditorInfo::IME_ACTION_SEND;
            break;
        }

        int newOptions = (currentIMEOptions & ~(int)EditorInfo::IME_MASK_ACTION) | imeOption;
        _jEditText.setImeOptions(newOptions);
    }

    void TextFieldCore::setPlaceholder(const Text &text)
    {
        std::visit(
            [&jView = this->_jEditText](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, std::string>) {
                    // Remove '\r' as android treats them as a space
                    std::string textToSet = arg;
                    textToSet.erase(
                        std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                        textToSet.end());
                    jView.setHint(textToSet);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto attrString = std::dynamic_pointer_cast<bdn::android::AttributedString>(arg)) {
                        jView.setHint(attrString->spanned());
                    }
                }
            },
            text);

        markDirty();
        scheduleLayout();
    }

    void TextFieldCore::updateInputType()
    {
        int newInputType = textInputTypeToInt(textInputType.get());
        newInputType |= autocorrectionTypeToInt(autocorrectionType.get());

        _jEditText.setInputType(newInputType);
    }

    int TextFieldCore::textInputTypeToInt(const TextInputType textInputType)
    {
        switch (textInputType) {
        case TextInputType::Text:
            return (int)InputType::TYPE_CLASS_TEXT |
                   (obscureInput.get() ? (int)InputType::TYPE_TEXT_VARIATION_PASSWORD : 0);
        case TextInputType::Number:
            return (int)InputType::TYPE_CLASS_NUMBER | (int)InputType::TYPE_NUMBER_FLAG_DECIMAL |
                   (int)InputType::TYPE_NUMBER_FLAG_SIGNED |
                   (obscureInput.get() ? (int)InputType::TYPE_NUMBER_VARIATION_PASSWORD : 0);
        case TextInputType::Phone:
            return (int)InputType::TYPE_CLASS_PHONE;
        case TextInputType::EMail:
            return (int)InputType::TYPE_CLASS_TEXT | (int)InputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
        case TextInputType::URL:
            return (int)InputType::TYPE_CLASS_TEXT | (int)InputType::TYPE_TEXT_VARIATION_URI;
        case TextInputType::MultiLine:
            return (int)InputType::TYPE_CLASS_TEXT | (int)InputType::TYPE_TEXT_VARIATION_LONG_MESSAGE;
        case TextInputType::DateTime:
            return (int)InputType::TYPE_CLASS_DATETIME;
        }

        return 0;
    }

    int TextFieldCore::autocorrectionTypeToInt(const AutocorrectionType autocorrectionType)
    {
        switch (autocorrectionType) {
        case AutocorrectionType::No:
            return (int)InputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS;
        case AutocorrectionType::Yes:
        case AutocorrectionType::Default:
            return (int)InputType::TYPE_TEXT_FLAG_AUTO_COMPLETE | (int)InputType::TYPE_TEXT_FLAG_AUTO_CORRECT;
        }

        return 0;
    }
}
