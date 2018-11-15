#ifndef BDN_ANDROID_JNativeDispatcher_H_
#define BDN_ANDROID_JNativeDispatcher_H_

#include <bdn/android/JLooper.h>
#include <bdn/java/JNativeOnceRunnable.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java io.boden.android.NativeDispatcher class.
         *
         * */
        class JNativeDispatcher : public bdn::java::JObject
        {
          private:
            static bdn::java::Reference newInstance_(JLooper looper)
            {
                static bdn::java::MethodId constructorId;

                return getStaticClass_().newInstance_(constructorId, looper);
            }

          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeDispatcher(const bdn::java::Reference &javaRef) : JObject(javaRef) {}

            explicit JNativeDispatcher(JLooper looper) : JObject(newInstance_(looper)) {}

            void enqueue(double delaySeconds, std::function<void()> func, bool idlePriority)
            {
                bdn::java::JNativeOnceRunnable runnable([func]() {
                    try {
                        func();
                    }
                    catch (DanglingFunctionError &) {
                        // ignore. This means that func is a weak method and
                        // that the corresponding object has been destroyed.
                    }
                });

                static bdn::java::MethodId methodId;

                return invoke_<void>(getStaticClass_(), methodId, "enqueue", delaySeconds,
                                     (bdn::java::JNativeRunnable &)runnable, idlePriority);
            }

            void createTimer(double intervalSeconds, IBase *pTimerData)
            {
                bdn::java::JNativeStrongPointer timerData(pTimerData);

                static bdn::java::MethodId methodId;

                return invoke_<void>(getStaticClass_(), methodId, "createTimer", intervalSeconds, timerData);
            }

            void dispose()
            {
                static bdn::java::MethodId methodId;

                return invoke_<void>(getStaticClass_(), methodId, "dispose");
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
            static bdn::java::JClass &getStaticClass_()
            {
                static bdn::java::JClass cls("io/boden/android/NativeDispatcher");

                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}

#endif
