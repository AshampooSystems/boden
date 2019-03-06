#pragma once

#include <bdn/android/wrapper/Handler.h>
#include <bdn/android/wrapper/Looper.h>

namespace bdn::android::wrapper
{

    /** Accessor for Java io.boden.android.NativeHandler class.
     *
     *  There is a static singleton of NativeHandler on the java side that
     * can be accessed via getMainNativeHandler().
     * */
    class NativeHandler : public Handler
    {
      public:
        /** @param javaRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit NativeHandler(const bdn::java::Reference &javaRef) : Handler(javaRef) {}

        static NativeHandler getMainNativeHandler()
        {
            static bdn::java::MethodId methodId;

            return invokeStatic_<NativeHandler>(getStaticClass_(), methodId, "getMainNativeHandler");
        }

        /** Returns the JClass object for this class.
         *
         *  Note that the returned class object is not necessarily unique
         * for the whole process. You might get different objects if this
         * function is called from different shared libraries.
         *
         *  If you want to check for type equality then you should compare
         * the type name (see getTypeName() )
         *  */
        static bdn::java::wrapper::Class &getStaticClass_()
        {
            static bdn::java::wrapper::Class cls("io/boden/android/NativeHandler");

            return cls;
        }

        bdn::java::wrapper::Class &getClass_() override { return getStaticClass_(); }
    };
}
