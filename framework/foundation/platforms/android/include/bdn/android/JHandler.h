#pragma once

#include <bdn/java/JObject.h>
#include <bdn/java/JRunnable.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java android.os.Handler objects.*/
        class JHandler : public bdn::java::JObject
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JHandler(const bdn::java::Reference &javaRef) : JObject(javaRef) {}

            /** Causes the Runnable object to be added to the message queue.
             *  The runnable will be run on the thread to which this handler is
             * attached.
             *
             *  Returns true if the Runnable was successfully placed in to the
             * message queue. Returns false on failure, usually because the
             * looper processing the message queue is exiting.
             *  */
            bool post(bdn::java::JRunnable runnable)
            {
                static bdn::java::MethodId methodId;

                return invoke_<bool>(getStaticClass_(), methodId, "post", runnable);
            }

            /** Causes the Runnable r to be added to the message queue, to be
             * run after the specified amount of time elapses. The runnable will
             * be run on the thread to which this handler is attached. The
             * time-base is uptimeMillis(). Time spent in deep sleep will add an
             * additional delay to execution.
             *
             *  Returns true if the Runnable was successfully placed in to the
             * message queue. Returns false on failure, usually because the
             * looper processing the message queue is exiting. Note that a
             *  result of true does not mean the Runnable will be processed --
             * if the looper is quit before the delivery time of the message
             * occurs then the message will be dropped.
             *  */
            bool postDelayed(bdn::java::JRunnable runnable, int64_t delayMillis)
            {
                static bdn::java::MethodId methodId;

                return invoke_<bool>(getStaticClass_(), methodId, "postDelayed", runnable, delayMillis);
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
                static bdn::java::JClass cls("android/os/Handler");

                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}
