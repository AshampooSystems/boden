#pragma once

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for the Java class android.R.style. */
        class JRStyle : public bdn::java::JObject
        {
          public:
            /** Constructor deleted. This class cannot be instantiated. It only
             * has static constants.*/
            JRStyle() = delete;

            /** Returns the JClass object for this class.
             *
             *  Note that the returned class object is not necessarily unique
             * for the whole process. You might get different objects if this
             * function is called from different shared libraries.
             *
             *  If you want to check for type equality then you should compare
             * the type name (see getTypeName() )
             *  */
            static bdn::java::JClass &getStaticClass_()
            {
                static bdn::java::JClass cls("android/R$style");

                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }

            enum
            {
                TextAppearance_Small = 0x01030046,
                TextAppearance_Medium = 0x01030044,
                TextAppearance_Large = 0x01030042
            };
        };
    }
}
