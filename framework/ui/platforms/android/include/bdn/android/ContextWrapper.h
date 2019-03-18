#pragma once

#include <bdn/android/wrapper/Context.h>

#include <utility>

namespace bdn::android
{
    class ContextWrapper
    {
      public:
        ContextWrapper() = delete;
        ContextWrapper(wrapper::Context ctxt) : _context(std::move(ctxt)) {}

      public:
        const wrapper::Context &getContext() const { return _context; }

      private:
        wrapper::Context _context;
    };
}
