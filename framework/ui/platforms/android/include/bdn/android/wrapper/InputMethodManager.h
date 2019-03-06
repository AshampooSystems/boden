#pragma once

#include <bdn/android/wrapper/IBinder.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kInputMethodManagerClassName[] = "android/view/inputmethod/InputMethodManager";

    class InputMethodManager : public java::wrapper::JTObject<kInputMethodManagerClassName>
    {
      public:
        using java::wrapper::JTObject<kInputMethodManagerClassName>::JTObject;

      public:
        JavaMethod<bool(IBinder, int)> hideSoftInputFromWindow{this, "hideSoftInputFromWindow"};
    };
}
