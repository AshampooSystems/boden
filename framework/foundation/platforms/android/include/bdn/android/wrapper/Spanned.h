#pragma once

#include <bdn/java/wrapper/CharSequence.h>

namespace bdn::android::wrapper
{
    class Spanned : public bdn::java::wrapper::CharSequence
    {
      public:
        explicit Spanned(const bdn::java::Reference &objectRef) : CharSequence(objectRef) {}

        static bdn::java::wrapper::Class &getStaticClass_()
        {
            static bdn::java::wrapper::Class cls("android/text/Spanned");
            return cls;
        }

        bdn::java::wrapper::Class &getClass_() override { return getStaticClass_(); }
    };
}
