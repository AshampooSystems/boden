#include <bdn/init.h>
#include <bdn/android/appEntry.h>

#include <bdn/android/AppRunner.h>
#include <bdn/android/JIntent.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace android
    {

        void appEntry(
            const std::function<P<AppControllerBase>()> &appControllerCreator,
            JNIEnv *pEnv, jobject rawIntent)
        {
            bdn::platformEntryWrapper(
                [&]() {
                    bdn::android::JIntent intent(
                        bdn::java::Reference::convertExternalLocal(rawIntent));

                    bdn::P<bdn::android::AppRunner> pAppRunner =
                        bdn::newObj<bdn::android::AppRunner>(
                            appControllerCreator, intent);
                    _setAppRunner(pAppRunner);

                    pAppRunner->entry();
                },
                true, pEnv);
        }
    }
}
