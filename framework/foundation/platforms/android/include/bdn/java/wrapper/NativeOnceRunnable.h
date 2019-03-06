#pragma once

#include <bdn/java/wrapper/NativeRunnable.h>

namespace bdn::java::wrapper
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

    constexpr const char kNativeOnceRunnableClassName[] = "io/boden/java/NativeOnceRunnable";

    class NativeOnceRunnable : public BaseNativeRunnable<kNativeOnceRunnableClassName>
    {
      public:
        using BaseNativeRunnable<kNativeOnceRunnableClassName>::BaseNativeRunnable;
    };
}
