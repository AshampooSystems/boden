#pragma once

#include <bdn/android/wrapper/EditText.h>
#include <bdn/android/wrapper/InputMethodManager.h>
#include <bdn/android/wrapper/KeyEvent.h>
#include <bdn/android/wrapper/NativeEditTextTextWatcher.h>
#include <bdn/android/wrapper/NativeTextViewOnEditorActionListener.h>

#include <bdn/TextField.h>

#include <bdn/android/ViewCore.h>
#include <bdn/debug.h>

namespace bdn::android
{

    class TextFieldCore : public ViewCore, virtual public bdn::TextField::Core
    {
      public:
        TextFieldCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        // Called by Java (via JNativeEditTextTextWatcher)
        void beforeTextChanged(const String &string, int start, int count, int after);

        // Called by Java (via JNativeEditTextTextWatcher)
        void onTextChanged(const String &string, int start, int before, int count);

        // Called by Java (via JNativeEditTextTextWatcher)
        void afterTextChanged();

        bool onEditorAction(int actionId, const wrapper::KeyEvent &keyEvent);

      private:
        mutable wrapper::EditText _jEditText;
        wrapper::NativeEditTextTextWatcher _watcher;
        wrapper::NativeTextViewOnEditorActionListener _onEditorActionListener;
    };
}
