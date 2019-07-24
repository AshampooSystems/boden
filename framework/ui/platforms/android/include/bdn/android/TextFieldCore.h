#pragma once

#include <bdn/android/wrapper/AppCompatEditText.h>
#include <bdn/android/wrapper/InputMethodManager.h>
#include <bdn/android/wrapper/KeyEvent.h>
#include <bdn/android/wrapper/NativeEditText.h>
#include <bdn/android/wrapper/NativeEditTextTextWatcher.h>
#include <bdn/android/wrapper/NativeTextViewOnEditorActionListener.h>

#include <bdn/ui/TextField.h>

#include <bdn/android/ViewCore.h>
#include <bdn/debug.h>

namespace bdn::ui::android
{
    class TextFieldCore : public ViewCore, virtual public TextField::Core
    {
      public:
        TextFieldCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        // Called by Java (via JNativeEditTextTextWatcher)
        void beforeTextChanged(const String &string, int start, int count, int after);

        // Called by Java (via JNativeEditTextTextWatcher)
        void onTextChanged(const String &string, int start, int before, int count);

        // Called by Java (via JNativeEditTextTextWatcher)
        void afterTextChanged();

        bool onEditorAction(int actionId, const bdn::android::wrapper::KeyEvent &keyEvent);

      public:
        void setFont(const Font &font);
        void setAutocorrectionType(const AutocorrectionType autocorrectionType);
        void setReturnKeyType(const ReturnKeyType returnKeyType);

      private:
        mutable bdn::android::wrapper::NativeEditText _jEditText;
        bdn::android::wrapper::NativeEditTextTextWatcher _watcher;
        bdn::android::wrapper::NativeTextViewOnEditorActionListener _onEditorActionListener;
    };
}
