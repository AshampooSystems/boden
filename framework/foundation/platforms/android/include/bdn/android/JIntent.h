#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JBundle.h>

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
            using java::JTObject<kIntentClassName>::JTObject;

            java::Method<String()> getAction{this, "getAction"};
            java::Method<JBundle()> getExtras{this, "getExtras"};
        };
    }
}
