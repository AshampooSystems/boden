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
          private:
            static std::shared_ptr<JEditText> _createJEditText(std::shared_ptr<TextField> outer)
            {
                // We need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                std::shared_ptr<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("TextFieldCore instance requested for a TextField that "
                                           "does not have a parent.");

                std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("TextFieldCore instance requested for a TextField with "
                                           "core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return std::make_shared<JEditText>(context);
            }

          public:
            TextFieldCore(std::shared_ptr<TextField> outerTextField)
                : ViewCore(outerTextField, _createJEditText(outerTextField))
            {
                _jEditText = std::dynamic_pointer_cast<JEditText>(getJViewPtr());
                _jEditText->setSingleLine(true);

                _watcher = std::make_shared<bdn::android::JNativeEditTextTextWatcher>(*_jEditText);
                _jEditText->addTextChangedListener(*_watcher);

                _onEditorActionListener = std::make_shared<bdn::android::JNativeTextViewOnEditorActionListener>();
                _jEditText->setOnEditorActionListener(*_onEditorActionListener);

                setText(outerTextField->text);
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
                    _jEditText->getContext().getSystemService(JContext::INPUT_METHOD_SERVICE).getRef_());
                inputManager.hideSoftInputFromWindow(_jEditText->getWindowToken(), 0);

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
                return _jEditText->getTextSize() / getUiScaleFactor();
            }

          private:
            std::shared_ptr<JEditText> _jEditText;
            std::shared_ptr<JNativeEditTextTextWatcher> _watcher;
            std::shared_ptr<JNativeTextViewOnEditorActionListener> _onEditorActionListener;
        };
    }
}
