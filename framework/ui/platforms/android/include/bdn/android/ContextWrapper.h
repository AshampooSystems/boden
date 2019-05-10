#pragma once

#include <bdn/android/wrapper/Context.h>

#include <utility>

namespace bdn::ui::android
{
    class ContextWrapper
    {
      public:
        ContextWrapper() = delete;
        ContextWrapper(bdn::android::wrapper::Context ctxt) : _context(std::move(ctxt)) {}

      public:
        const bdn::android::wrapper::Context &getContext() const { return _context; }

      private:
        bdn::android::wrapper::Context _context;
    };
}
