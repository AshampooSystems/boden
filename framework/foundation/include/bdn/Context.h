#pragma once

#include <bdn/platform.h>
#include <memory>

namespace bdn
{

    class Context : std::enable_shared_from_this<Context>
    {
    };

#ifdef BDN_PLATFORM_ANDROID
    namespace android
    {
        class ContextWrapper;

        class Context : public bdn::Context
        {
          public:
            Context(std::unique_ptr<android::ContextWrapper> &&contextWrapper);

          public:
            const std::unique_ptr<android::ContextWrapper> _contextWrapper;
        };
    }
#endif
}
