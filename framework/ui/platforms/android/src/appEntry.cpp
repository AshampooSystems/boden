
#include <bdn/android/AttributedString.h>
#include <bdn/android/UIApplication.h>
#include <bdn/android/appEntry.h>
#include <bdn/android/wrapper/Intent.h>
#include <bdn/entry.h>

namespace bdn::ui::android
{

    void appEntry(const std::function<std::shared_ptr<ApplicationController>()> &appControllerCreator, JNIEnv *env,
                  jobject rawIntent)
    {
        bdn::platformEntryWrapper(
            [&]() {
                bdn::android::wrapper::Intent intent(bdn::java::Reference::convertExternalLocal(rawIntent));

                std::shared_ptr<android::UIApplication> app =
                    std::make_shared<android::UIApplication>(appControllerCreator, intent);
                app->init();
                app->entry();
            },
            true, env);
    }
}
