#ifndef BDN_ANDROID_TextFieldCore_H_
#define BDN_ANDROID_TextFieldCore_H_

#include <bdn/android/JEditText.h>
#include <bdn/android/JKeyEvent.h>
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
            static P<JEditText> _createJEditText(TextField *pOuter)
            {
                // We need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> pParent = pOuter->getParentView();
                if (pParent == nullptr)
                    throw ProgrammingError(
                        "TextFieldCore instance requested for a TextField that "
                        "does not have a parent.");

                P<ViewCore> pParentCore =
                    cast<ViewCore>(pParent->getViewCore());
                if (pParentCore == nullptr)
                    throw ProgrammingError(
                        "TextFieldCore instance requested for a TextField with "
                        "core-less parent.");

                JContext context = pParentCore->getJView().getContext();

                return newObj<JEditText>(context);
            }

          public:
            TextFieldCore(TextField *pOuterTextField)
                : ViewCore(pOuterTextField, _createJEditText(pOuterTextField))
            {
                _pJEditText = cast<JEditText>(&getJView());
                _pJEditText->setSingleLine(true);

                _pWatcher = newObj<bdn::android::JNativeEditTextTextWatcher>(
                    *_pJEditText);
                _pJEditText->addTextChangedListener(*_pWatcher);

                _pOnEditorActionListener = newObj<
                    bdn::android::JNativeTextViewOnEditorActionListener>();
                _pJEditText->setOnEditorActionListener(
                    *_pOnEditorActionListener);

                setText(pOuterTextField->text());
            }

            JEditText &getJEditText() { return *_pJEditText; }

            void setText(const String &text) override
            {
                _pJEditText->removeTextChangedListener(*_pWatcher);
                String currentText = _pJEditText->getText();
                if (text != currentText) {
                    _pJEditText->setText(text);
                }
                _pJEditText->addTextChangedListener(*_pWatcher);
            }

          public:
            // Called by Java (via JNativeEditTextTextWatcher)
            void beforeTextChanged(String string, int start, int count,
                                   int after)
            {}

            // Called by Java (via JNativeEditTextTextWatcher)
            void onTextChanged(String string, int start, int before, int count)
            {}

            // Called by Java (via JNativeEditTextTextWatcher)
            void afterTextChanged()
            {
                String newText = _pJEditText->getText();
                P<TextField> outerTextField =
                    cast<TextField>(getOuterViewIfStillAttached());
                if (outerTextField) {
                    outerTextField->setText(newText);
                }
            }

            bool onEditorAction(int actionId, JKeyEvent keyEvent)
            {
                P<TextField> outerTextField =
                    cast<TextField>(getOuterViewIfStillAttached());
                if (outerTextField) {
                    outerTextField->submit();
                }
                return true;
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _pJEditText->getTextSize() / getUiScaleFactor();
            }

          private:
            P<JEditText> _pJEditText;
            P<JNativeEditTextTextWatcher> _pWatcher;
            P<JNativeTextViewOnEditorActionListener> _pOnEditorActionListener;
        };
    }
}

#endif // BDN_ANDROID_TextFieldCore_H_
