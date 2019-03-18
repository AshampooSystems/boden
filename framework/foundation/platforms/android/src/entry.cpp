
#include <bdn/entry.h>

#include <bdn/java/Env.h>

namespace bdn
{
    void platformEntryWrapper(const std::function<void()> &function, bool /*unused*/, void *platformSpecific)
    {
        auto env = static_cast<JNIEnv *>(platformSpecific);
        bdn::java::Env::get().jniBlockBegun(env);

        function();
    }
}
