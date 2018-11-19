#ifndef BDN_JAVA_JNativeOnceRunnable_H_
#define BDN_JAVA_JNativeOnceRunnable_H_

#include <bdn/java/JNativeRunnable.h>

namespace bdn
{
    namespace java
    {

        /** Accessor for Java-side io.boden.java.NativeOnceRunnable objects.
         *
         *  These objects provide a way for a piece of native code to be
         * executed by a Java-side runnable object.
         *
         *
         *  In contrast to JNativeRunnable, the runnable object immediately
         * releases the native callable object after run() is called once. That
         * has the advantage that the object does not remain referenced until
         *  the Java garbage collector eventually removes it. So
         * JNativeOnceRunnable provides better performance and more
         * deterministic behaviour for cases when the runnable is only used
         * once. *
         *
         *  If run is called multiple times then subsequence run() calls have no
         * effect.
         * */
        class JNativeOnceRunnable : public JNativeRunnable
        {
          private:
            static Reference newInstance_(ISimpleCallable *callable)
            {
                static MethodId constructorId;

                JNativeStrongPointer wrappedCallable(callable);

                return getStaticClass_().newInstance_(constructorId, wrappedCallable);
            }

            static Reference newInstance_(const std::function<void()> &func)
            {
                return newInstance_(newObj<SimpleFunctionCallable>(func));
            }

          public:
            explicit JNativeOnceRunnable(const std::function<void()> &func) : JNativeRunnable(newInstance_(func)) {}

            explicit JNativeOnceRunnable(ISimpleCallable *callable) : JNativeRunnable(newInstance_(callable)) {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeOnceRunnable(const Reference &objectRef) : JNativeRunnable(objectRef) {}

            /** Returns the JClass object for this class.
             *
             *  Note that the returned class object is not necessarily unique
             * for the whole process. You might get different objects if this
             * function is called from different shared libraries.
             *
             *  If you want to check for type equality then you should compare
             * the type name (see getTypeName() )
             *  */
            static JClass &getStaticClass_()
            {
                static JClass cls("io/boden/java/NativeOnceRunnable");

                return cls;
            }

            JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}

#endif
