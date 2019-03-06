#pragma once

#include <bdn/android/wrapper/Bundle.h>
#include <bdn/android/wrapper/Uri.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kIntentClassName[] = "android/content/Intent";

    class Intent : public java::wrapper::JTObject<kIntentClassName>
    {
      public:
        constexpr static const char ACTION_MAIN[] = "android.intent.action.MAIN";
        constexpr static const char ACTION_VIEW[] = "android.intent.action.VIEW";

      public:
        static bdn::java::Reference createIntentInstance(String string, Uri uri)
        {
            static bdn::java::MethodId constructorId;
            return javaClass().newInstance_(constructorId, string, uri);
        }

        Intent(String string, Uri uri) : JTObject<kIntentClassName>(createIntentInstance(string, uri)) {}

      public:
        using JTObject<kIntentClassName>::JTObject;

        JavaMethod<String()> getAction{this, "getAction"};
        JavaMethod<Bundle()> getExtras{this, "getExtras"};
    };
}
