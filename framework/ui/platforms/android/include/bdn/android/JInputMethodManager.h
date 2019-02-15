#pragma once

#include <bdn/android/JIBinder.h>
#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kInputMethodManagerClassName[] = "android/view/inputmethod/InputMethodManager";

        class JInputMethodManager : public java::JTObject<kInputMethodManagerClassName>
        {
          public:
            using java::JTObject<kInputMethodManagerClassName>::JTObject;

          public:
            java::Method<bool(JIBinder, int)> hideSoftInputFromWindow{this, "hideSoftInputFromWindow"};
        };
    }
}
