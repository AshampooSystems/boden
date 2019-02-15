#pragma once

#include <bdn/java/JNativeStrongPointer.h>
#include <bdn/java/JRunnable.h>

#include <bdn/SimpleFunctionCallable.h>

#include <functional>

namespace bdn
{
    namespace java
    {

        /** Accessor for Java-side io.boden.java.NativeRunnable objects.
         *
         *  These objects provide a way for a piece of native code to be
         * executed by a Java-side runnable object.
         *
         *  Consider using JNativeOnceRunnable instead if the runnable object is
         * only used once.
         *
         * */
        class JNativeRunnable : public bdn::java::JRunnable
        {
          private:
            static Reference newInstance_(std::shared_ptr<ISimpleCallable> pCallable)
            {
                static MethodId constructorId;

                JNativeStrongPointer wrappedCallable(std::dynamic_pointer_cast<Base>(pCallable));

                return getStaticClass_().newInstance_(constructorId, wrappedCallable);
            }

            static Reference newInstance_(const std::function<void()> &func)
            {
                return newInstance_(std::make_shared<SimpleFunctionCallable>(func));
            }

          public:
            explicit JNativeRunnable(const std::function<void()> &func) : JRunnable(newInstance_(func)) {}

            explicit JNativeRunnable(std::shared_ptr<ISimpleCallable> pCallable) : JRunnable(newInstance_(pCallable)) {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeRunnable(const Reference &objectRef) : JRunnable(objectRef) {}

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
                static JClass cls("io/boden/java/NativeRunnable");

                return cls;
            }

            JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}
