#pragma once

#include <bdn/android/JBundle.h>
#include <bdn/android/JUri.h>
#include <bdn/java/JObject.h>

namespace bdn::android
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

        JIntent(String string, JUri uri) : JTObject<kIntentClassName>(createIntentInstance(string, uri)) {}

      public:
        using JTObject<kIntentClassName>::JTObject;

        java::Method<String()> getAction{this, "getAction"};
        java::Method<JBundle()> getExtras{this, "getExtras"};
    };
}
