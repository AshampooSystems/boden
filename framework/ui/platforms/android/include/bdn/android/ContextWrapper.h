#pragma once

#include <bdn/android/wrapper/Context.h>

namespace bdn::android
{
    class ContextWrapper
    {
      public:
        ContextWrapper() = delete;
        ContextWrapper(const wrapper::Context &ctxt) : _context(ctxt) {}

      public:
        const wrapper::Context &getContext() const { return _context; }

      private:
        wrapper::Context _context;
    };
}
