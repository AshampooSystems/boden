#pragma once

#include <bdn/android/wrapper/Spanned.h>
#include <string>

namespace bdn::android::wrapper
{
    class SpannableStringBuilder : public Spanned
    {
      private:
        static bdn::java::Reference newInstance_(const bdn::java::wrapper::CharSequence &s)
        {
            static bdn::java::MethodId constructorId;
            return getStaticClass_().newInstance_(constructorId, s);
        }

      public:
        SpannableStringBuilder(const bdn::java::wrapper::CharSequence &text) : Spanned(newInstance_(text)) {}

        explicit SpannableStringBuilder(const bdn::java::Reference &objectRef) : Spanned(objectRef) {}

        static bdn::java::wrapper::Class &getStaticClass_()
        {
            static bdn::java::wrapper::Class cls("android/text/SpannableStringBuilder");
            return cls;
        }

        bdn::java::wrapper::Class &getClass_() override { return getStaticClass_(); }

      public:
        JavaMethod<void(java::wrapper::Object, int, int, int)> setSpan{this, "setSpan"};
        JavaMethod<int()> length{this, "length"};
    };
}
