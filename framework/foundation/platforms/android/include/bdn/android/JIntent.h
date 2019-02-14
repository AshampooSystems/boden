#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JBundle.h>
#include <bdn/android/JUri.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kIntentClassName[] = "android/content/Intent";

        class JIntent : public java::JTObject<kIntentClassName>
        {
          public:
            constexpr static const char ACTION_MAIN[] = "android.intent.action.MAIN";
            constexpr static const char ACTION_VIEW[] = "android.intent.action.VIEW";

          public:
            static bdn::java::Reference createIntentInstance(String string, JUri uri)
            {
                static bdn::java::MethodId constructorId;
                return javaClass().newInstance_(constructorId, string, uri);
            }

            JIntent(String string, JUri uri) : JTObject(createIntentInstance(string, uri)) {}

          public:
            using java::JTObject<kIntentClassName>::JTObject;

            java::Method<String()> getAction{this, "getAction"};
            java::Method<JBundle()> getExtras{this, "getExtras"};
        };
    }
}
