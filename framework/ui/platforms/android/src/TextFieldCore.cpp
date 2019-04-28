#include <bdn/android/TextFieldCore.h>

namespace bdn::detail
{
    CORE_REGISTER(TextField, bdn::android::TextFieldCore, TextField)
}

namespace bdn::android
{
    TextFieldCore::TextFieldCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::AppCompatEditText>(viewCoreFactory)),
          _jEditText(getJViewAS<wrapper::EditText>()), _watcher(_jEditText.cast<wrapper::TextView>())
    {
        _jEditText.setSingleLine(true);

        _jEditText.addTextChangedListener(_watcher.cast<wrapper::TextWatcher>());

        _jEditText.setOnEditorActionListener(_onEditorActionListener.cast<wrapper::OnEditorActionListener>());

        text.onChange() += [=](auto &property) {
            _jEditText.removeTextChangedListener(_watcher.cast<wrapper::TextWatcher>());
            String currentText = _jEditText.getText();
            if (property.get() != currentText) {
                _jEditText.setText(property.get());
            }
            _jEditText.addTextChangedListener(_watcher.cast<wrapper::TextWatcher>());
        };
    }

    void TextFieldCore::beforeTextChanged(const String &string, int start, int count, int after) {}

    void TextFieldCore::onTextChanged(const String &string, int start, int before, int count) {}

    void TextFieldCore::afterTextChanged()
    {
        String newText = _jEditText.getText();
        text = newText;
    }

    bool TextFieldCore::onEditorAction(int actionId, const wrapper::KeyEvent &keyEvent)
    {
        // hide virtual keyboard
        wrapper::InputMethodManager inputManager(
            _jEditText.getContext().getSystemService(wrapper::Context::INPUT_METHOD_SERVICE).getRef_());
        inputManager.hideSoftInputFromWindow(_jEditText.getWindowToken(), 0);

        submitCallback.fire();

        return true;
    }
}
