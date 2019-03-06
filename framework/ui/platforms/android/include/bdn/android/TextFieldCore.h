#pragma once

#include <bdn/android/wrapper/EditText.h>
#include <bdn/android/wrapper/InputMethodManager.h>
#include <bdn/android/wrapper/KeyEvent.h>
#include <bdn/android/wrapper/NativeEditTextTextWatcher.h>
#include <bdn/android/wrapper/NativeTextViewOnEditorActionListener.h>

#include <bdn/ITextFieldCore.h>
#include <bdn/TextField.h>
#include <bdn/android/ViewCore.h>
#include <bdn/debug.h>

namespace bdn::android
{

    class TextFieldCore : public ViewCore, virtual public ITextFieldCore
    {
      public:
        TextFieldCore(std::shared_ptr<TextField> outerTextField)
            : ViewCore(outerTextField, createAndroidViewClass<wrapper::EditText>(outerTextField)),
              _jEditText(getJViewAS<wrapper::EditText>()), _watcher(_jEditText.cast<wrapper::TextView>())
        {
            _jEditText.setSingleLine(true);

            _jEditText.addTextChangedListener(_watcher.cast<wrapper::TextWatcher>());

            _jEditText.setOnEditorActionListener(_onEditorActionListener.cast<wrapper::OnEditorActionListener>());

            setText(outerTextField->text);
        }

        wrapper::EditText &getJEditText() { return _jEditText; }

        void setText(const String &text) override
        {
            _jEditText.removeTextChangedListener(_watcher.cast<wrapper::TextWatcher>());
            String currentText = _jEditText.getText();
            if (text != currentText) {
                _jEditText.setText(text);
            }
            _jEditText.addTextChangedListener(_watcher.cast<wrapper::TextWatcher>());
        }

      public:
        // Called by Java (via JNativeEditTextTextWatcher)
        void beforeTextChanged(String string, int start, int count, int after) {}

        // Called by Java (via JNativeEditTextTextWatcher)
        void onTextChanged(String string, int start, int before, int count) {}

        // Called by Java (via JNativeEditTextTextWatcher)
        void afterTextChanged()
        {
            String newText = _jEditText.getText();
            std::shared_ptr<TextField> outerTextField = std::dynamic_pointer_cast<TextField>(outerView());
            if (outerTextField) {
                outerTextField->text = (newText);
            }
        }

        bool onEditorAction(int actionId, wrapper::KeyEvent keyEvent)
        {
            // hide virtual keyboard
            wrapper::InputMethodManager inputManager(
                _jEditText.getContext().getSystemService(wrapper::Context::INPUT_METHOD_SERVICE).getRef_());
            inputManager.hideSoftInputFromWindow(_jEditText.getWindowToken(), 0);

            std::shared_ptr<TextField> outerTextField = std::dynamic_pointer_cast<TextField>(outerView());
            if (outerTextField) {
                outerTextField->submit();
            }
            return true;
        }

      protected:
        double getFontSizeDips() const override
        {
            // the text size is in pixels
            return _jEditText.getTextSize() / getUIScaleFactor();
        }

      private:
        mutable wrapper::EditText _jEditText;
        wrapper::NativeEditTextTextWatcher _watcher;
        wrapper::NativeTextViewOnEditorActionListener _onEditorActionListener;
    };
}
