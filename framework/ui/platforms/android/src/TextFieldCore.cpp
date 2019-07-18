#include <bdn/android/TextFieldCore.h>
#include <bdn/android/wrapper/InputType.h>

using bdn::android::wrapper::Context;
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
            String currentText = _jEditText.getText();
            if (property.get() != currentText) {
                _jEditText.setText(property.get());
            }
            _jEditText.addTextChangedListener(_watcher.cast<TextWatcher>());
        };

        font.onChange() += [=](auto &property) { setFont(property.get()); };

        autocorrectionType.onChange() += [=](auto &property) { setAutocorrectionType(property.get()); };
    }

    void TextFieldCore::beforeTextChanged(const String &string, int start, int count, int after) {}

    void TextFieldCore::onTextChanged(const String &string, int start, int before, int count) {}

    void TextFieldCore::afterTextChanged()
    {
        String newText = _jEditText.getText();
        text = newText;
    }

    bool TextFieldCore::onEditorAction(int actionId, const bdn::android::wrapper::KeyEvent &keyEvent)
    {
        bdn::android::wrapper::InputMethodManager inputManager(
            _jEditText.getContext().getSystemService(Context::INPUT_METHOD_SERVICE).getRef_());
        inputManager.hideSoftInputFromWindow(_jEditText.getWindowToken(), 0);

        submitCallback.fire();

        return true;
    }

    void TextFieldCore::setFont(const Font &font)
    {
        _jEditText.setFont(font.family, (int)font.size.type, font.size.value, font.weight,
                           font.style == Font::Style::Italic);
    }

    void TextFieldCore::setAutocorrectionType(const AutocorrectionType autocorrectionType)
    {
        int currentInputType = _jEditText.getInputType();

        switch (autocorrectionType) {
        case AutocorrectionType::No:
            _jEditText.setInputType((currentInputType & ~((int)InputType::TYPE_TEXT_FLAG_AUTO_COMPLETE |
                                                          (int)InputType::TYPE_TEXT_FLAG_AUTO_CORRECT)) |
                                    (int)InputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS);
            break;
        case AutocorrectionType::Yes:
        case AutocorrectionType::Default:
            _jEditText.setInputType((currentInputType & ~(int)InputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS) |
                                    (int)InputType::TYPE_TEXT_FLAG_AUTO_COMPLETE |
                                    (int)InputType::TYPE_TEXT_FLAG_AUTO_CORRECT);
            break;
        }
    }
}
