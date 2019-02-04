#pragma once

#include <bdn/android/JEditText.h>
#include <bdn/android/JKeyEvent.h>
#include <bdn/android/JInputMethodManager.h>
#include <bdn/android/JNativeEditTextTextWatcher.h>
#include <bdn/android/JNativeTextViewOnEditorActionListener.h>

#include <bdn/android/ViewCore.h>
#include <bdn/ITextFieldCore.h>
#include <bdn/TextField.h>
#include <bdn/debug.h>

namespace bdn
{
    namespace android
    {

        class TextFieldCore : public ViewCore, virtual public ITextFieldCore
        {
          public:
            TextFieldCore(std::shared_ptr<TextField> outerTextField)
                : ViewCore(outerTextField, ViewCore::createAndroidViewClass<JEditText>(outerTextField)),
                  _jEditText(getJViewAS<JEditText>()), _watcher(_jEditText.cast<JTextView>())
            {
                _jEditText.setSingleLine(true);

                _jEditText.addTextChangedListener(_watcher.cast<JTextWatcher>());

                _jEditText.setOnEditorActionListener(_onEditorActionListener.cast<OnEditorActionListener>());

                setText(outerTextField->text);
            }

            JEditText &getJEditText() { return _jEditText; }

            void setText(const String &text) override
            {
                _jEditText.removeTextChangedListener(_watcher.cast<JTextWatcher>());
                String currentText = _jEditText.getText();
                if (text != currentText) {
                    _jEditText.setText(text);
                }
                _jEditText.addTextChangedListener(_watcher.cast<JTextWatcher>());
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
                std::shared_ptr<TextField> outerTextField =
                    std::dynamic_pointer_cast<TextField>(getOuterViewIfStillAttached());
                if (outerTextField) {
                    outerTextField->text = (newText);
                }
            }

            bool onEditorAction(int actionId, JKeyEvent keyEvent)
            {
                // hide virtual keyboard
                JInputMethodManager inputManager(
                    _jEditText.getContext().getSystemService(JContext::INPUT_METHOD_SERVICE).getRef_());
                inputManager.hideSoftInputFromWindow(_jEditText.getWindowToken(), 0);

                std::shared_ptr<TextField> outerTextField =
                    std::dynamic_pointer_cast<TextField>(getOuterViewIfStillAttached());
                if (outerTextField) {
                    outerTextField->submit();
                }
                return true;
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jEditText.getTextSize() / getUiScaleFactor();
            }

          private:
            mutable JEditText _jEditText;
            JNativeEditTextTextWatcher _watcher;
            JNativeTextViewOnEditorActionListener _onEditorActionListener;
        };
    }
}
