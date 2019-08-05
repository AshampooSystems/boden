#pragma once

#include <bdn/android/wrapper/Bundle.h>
#include <bdn/android/wrapper/Uri.h>
#include <bdn/java/wrapper/Object.h>

#include <utility>

namespace bdn::android::wrapper
{
    constexpr const char kIntentClassName[] = "android/content/Intent";

    class Intent : public java::wrapper::JTObject<kIntentClassName>
    {
      public:
        constexpr static const char ACTION_MAIN[] = "android.intent.action.MAIN";
        constexpr static const char ACTION_VIEW[] = "android.intent.action.VIEW";

      public:
        static bdn::java::Reference createIntentInstance(std::string string, Uri uri)
        {
            static bdn::java::MethodId constructorId;
            return javaClass().newInstance_(constructorId, std::move(string), std::move(uri));
        }

        Intent(std::string string, Uri uri)
            : JTObject<kIntentClassName>(createIntentInstance(std::move(string), std::move(uri)))
        {}

      public:
        using JTObject<kIntentClassName>::JTObject;

        JavaMethod<std::string()> getAction{this, "getAction"};
        JavaMethod<Bundle()> getExtras{this, "getExtras"};
    };
}
