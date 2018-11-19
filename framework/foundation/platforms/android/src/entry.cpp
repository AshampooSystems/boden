#include <bdn/init.h>
#include <bdn/entry.h>

#include <bdn/java/Env.h>

namespace bdn
{
    void platformEntryWrapper(std::function<void()> function, bool, void *platformSpecific)
    {
        auto env = static_cast<JNIEnv *>(platformSpecific);
        bdn::java::Env::get().jniBlockBegun(env);

        try {
            function();
        }
        catch (...) {
            // Just let exceptions through to the java side. We have a global
            // uncaught exception handler installed that will pass any exception
            // that is not handled by the java code to our own
            // unhandledException function.
            bdn::java::Env::get().setJavaSideException(std::current_exception());
        }
    }
}
