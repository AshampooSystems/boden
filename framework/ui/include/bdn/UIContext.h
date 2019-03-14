#pragma once

#include <bdn/Base.h>
#include <bdn/platform.h>
#include <memory>

namespace bdn
{

    class UIContext : public Base
    {
    };

#ifdef BDN_PLATFORM_ANDROID
    namespace android
    {
        class ContextWrapper;

        class UIContext : public bdn::UIContext
        {
          public:
            UIContext(std::unique_ptr<android::ContextWrapper> &&contextWrapper);

          public:
            const std::unique_ptr<android::ContextWrapper> _contextWrapper;
        };
    }
#endif
}
