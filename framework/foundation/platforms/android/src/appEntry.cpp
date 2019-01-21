
#include <bdn/android/appEntry.h>

#include <bdn/android/AppRunner.h>
#include <bdn/android/JIntent.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace android
    {

        void appEntry(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator, JNIEnv *env,
                      jobject rawIntent)
        {
            bdn::platformEntryWrapper(
                [&]() {
                    bdn::android::JIntent intent(bdn::java::Reference::convertExternalLocal(rawIntent));

                    std::shared_ptr<bdn::android::AppRunner> appRunner =
                        std::make_shared<bdn::android::AppRunner>(appControllerCreator, intent);
                    _setAppRunner(appRunner);

                    appRunner->entry();
                },
                true, env);
        }
    }
}
