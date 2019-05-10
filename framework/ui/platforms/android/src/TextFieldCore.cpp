#include <bdn/android/TextFieldCore.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(TextField, bdn::ui::android::TextFieldCore, TextField)
}

namespace bdn::ui::android
{
    TextFieldCore::TextFieldCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::AppCompatEditText>(viewCoreFactory)),
          _jEditText(getJViewAS<bdn::android::wrapper::EditText>()),
          _watcher(_jEditText.cast<bdn::android::wrapper::TextView>())
    {
        _jEditText.setSingleLine(true);

        _jEditText.addTextChangedListener(_watcher.cast<bdn::android::wrapper::TextWatcher>());

        _jEditText.setOnEditorActionListener(
            _onEditorActionListener.cast<bdn::android::wrapper::OnEditorActionListener>());

        text.onChange() += [=](auto &property) {
            _jEditText.removeTextChangedListener(_watcher.cast<bdn::android::wrapper::TextWatcher>());
            String currentText = _jEditText.getText();
            if (property.get() != currentText) {
                _jEditText.setText(property.get());
            }
            _jEditText.addTextChangedListener(_watcher.cast<bdn::android::wrapper::TextWatcher>());
        };
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
        // hide virtual keyboard
        bdn::android::wrapper::InputMethodManager inputManager(
            _jEditText.getContext().getSystemService(bdn::android::wrapper::Context::INPUT_METHOD_SERVICE).getRef_());
        inputManager.hideSoftInputFromWindow(_jEditText.getWindowToken(), 0);

        submitCallback.fire();

        return true;
    }
}
