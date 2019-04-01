#pragma once

#include <bdn/ISyncNotifier.h>
#include <bdn/NotifierBase.h>

namespace bdn
{

    /** A simple, fast synchrnous notifier. See ISyncNotifier.

        SimpleNotifier is **not** thread safe.
    */
    template <class... ARG_TYPES>
    class SimpleNotifier : public NotifierBase<bdn::nonatomic, ARG_TYPES...>, virtual public ISyncNotifier<ARG_TYPES...>
    {
      private:
        using BASE = NotifierBase<bdn::nonatomic, ARG_TYPES...>;

      public:
        SimpleNotifier() = default;

        ~SimpleNotifier() override = default;

        void notify(ARG_TYPES... args) override { BASE::doNotify(std::forward<ARG_TYPES>(args)...); }

      private:
    };
}
