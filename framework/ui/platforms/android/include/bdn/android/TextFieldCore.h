#pragma once

#include <bdn/android/wrapper/EditText.h>
#include <bdn/android/wrapper/InputMethodManager.h>
#include <bdn/android/wrapper/KeyEvent.h>
#include <bdn/android/wrapper/NativeEditTextTextWatcher.h>
#include <bdn/android/wrapper/NativeTextViewOnEditorActionListener.h>

#include <bdn/TextField.h>
#include <bdn/TextFieldCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/debug.h>

namespace bdn::android
{

    class TextFieldCore : public ViewCore, virtual public bdn::TextFieldCore
    {
      public:
        TextFieldCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
            : ViewCore(uiProvider, createAndroidViewClass<wrapper::EditText>(uiProvider)),
              _jEditText(getJViewAS<wrapper::EditText>()), _watcher(_jEditText.cast<wrapper::TextView>())
        {
            _jEditText.setSingleLine(true);

            _jEditText.addTextChangedListener(_watcher.cast<wrapper::TextWatcher>());

            _jEditText.setOnEditorActionListener(_onEditorActionListener.cast<wrapper::OnEditorActionListener>());

            text.onChange() += [=](auto va) {
                _jEditText.removeTextChangedListener(_watcher.cast<wrapper::TextWatcher>());
                String currentText = _jEditText.getText();
                if (va->get() != currentText) {
                    _jEditText.setText(va->get());
                }
                _jEditText.addTextChangedListener(_watcher.cast<wrapper::TextWatcher>());
            };
        }

        wrapper::EditText &getJEditText() { return _jEditText; }

      public:
        // Called by Java (via JNativeEditTextTextWatcher)
        void beforeTextChanged(String string, int start, int count, int after) {}

        // Called by Java (via JNativeEditTextTextWatcher)
        void onTextChanged(String string, int start, int before, int count) {}

        // Called by Java (via JNativeEditTextTextWatcher)
        void afterTextChanged()
        {
            String newText = _jEditText.getText();
            text = newText;
        }

        bool onEditorAction(int actionId, wrapper::KeyEvent keyEvent)
        {
            // hide virtual keyboard
            wrapper::InputMethodManager inputManager(
                _jEditText.getContext().getSystemService(wrapper::Context::INPUT_METHOD_SERVICE).getRef_());
            inputManager.hideSoftInputFromWindow(_jEditText.getWindowToken(), 0);

            submitCallback.fire();

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
