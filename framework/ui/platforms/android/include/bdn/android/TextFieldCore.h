#ifndef BDN_ANDROID_TextFieldCore_H_
#define BDN_ANDROID_TextFieldCore_H_

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

        class TextFieldCore : public ViewCore, BDN_IMPLEMENTS ITextFieldCore
        {
          private:
            static P<JEditText> _createJEditText(TextField *outer)
            {
                // We need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("TextFieldCore instance requested for a TextField that "
                                           "does not have a parent.");

                P<ViewCore> parentCore = cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("TextFieldCore instance requested for a TextField with "
                                           "core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return newObj<JEditText>(context);
            }

          public:
            TextFieldCore(TextField *outerTextField) : ViewCore(outerTextField, _createJEditText(outerTextField))
            {
                _jEditText = cast<JEditText>(&getJView());
                _jEditText->setSingleLine(true);

                _watcher = newObj<bdn::android::JNativeEditTextTextWatcher>(*_jEditText);
                _jEditText->addTextChangedListener(*_watcher);

                _onEditorActionListener = newObj<bdn::android::JNativeTextViewOnEditorActionListener>();
                _jEditText->setOnEditorActionListener(*_onEditorActionListener);

                setText(outerTextField->text());
            }

            JEditText &getJEditText() { return *_jEditText; }

            void setText(const String &text) override
            {
                _jEditText->removeTextChangedListener(*_watcher);
                String currentText = _jEditText->getText();
                if (text != currentText) {
                    _jEditText->setText(text);
                }
                _jEditText->addTextChangedListener(*_watcher);
            }

          public:
            // Called by Java (via JNativeEditTextTextWatcher)
            void beforeTextChanged(String string, int start, int count, int after) {}

            // Called by Java (via JNativeEditTextTextWatcher)
            void onTextChanged(String string, int start, int before, int count) {}

            // Called by Java (via JNativeEditTextTextWatcher)
            void afterTextChanged()
            {
                String newText = _jEditText->getText();
                P<TextField> outerTextField = cast<TextField>(getOuterViewIfStillAttached());
                if (outerTextField) {
                    outerTextField->setText(newText);
                }
            }

            bool onEditorAction(int actionId, JKeyEvent keyEvent)
            {
                // hide virtual keyboard
                JInputMethodManager inputManager(
                    _jEditText->getContext().getSystemService(JContext::INPUT_METHOD_SERVICE).getRef_());
                inputManager.hideSoftInputFromWindow(_jEditText->getWindowToken(), 0);

                P<TextField> outerTextField = cast<TextField>(getOuterViewIfStillAttached());
                if (outerTextField) {
                    outerTextField->submit();
                }
                return true;
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jEditText->getTextSize() / getUiScaleFactor();
            }

          private:
            P<JEditText> _jEditText;
            P<JNativeEditTextTextWatcher> _watcher;
            P<JNativeTextViewOnEditorActionListener> _onEditorActionListener;
        };
    }
}

#endif // BDN_ANDROID_TextFieldCore_H_
