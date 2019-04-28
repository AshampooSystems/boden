#pragma once

#include <bdn/platform.h>
#include <memory>

namespace bdn
{

    class UIContext : std::enable_shared_from_this<UIContext>
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
