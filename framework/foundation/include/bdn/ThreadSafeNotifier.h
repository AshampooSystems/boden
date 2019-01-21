#pragma once

#include <bdn/IAsyncNotifier.h>
#include <bdn/ISyncNotifier.h>
#include <bdn/NotifierBase.h>
#include <bdn/mainThread.h>

namespace bdn
{

    /** A thread safe notifier implementation. ThreadSafeNotifier supports both
       the IAsyncNotifier and the ISyncNotifier interfaces.

    */
    template <class... ARG_TYPES>
    class ThreadSafeNotifier : public NotifierBase<bdn::atomic, ARG_TYPES...>,
                               virtual public IAsyncNotifier<ARG_TYPES...>,
                               virtual public ISyncNotifier<ARG_TYPES...>
    {
      private:
        using BASE = NotifierBase<bdn::atomic, ARG_TYPES...>;

      public:
        ThreadSafeNotifier() {}

        ~ThreadSafeNotifier() {}

        std::shared_ptr<ThreadSafeNotifier<ARG_TYPES...>> shared_from_this()
        {
            return std::dynamic_pointer_cast<ThreadSafeNotifier<ARG_TYPES...>>(Base::shared_from_this());
        }

        void notify(ARG_TYPES... args) override { BASE::doNotify(std::forward<ARG_TYPES>(args)...); }

        void postNotification(ARG_TYPES... args) override
        {
            // see doc_input/notifier_internal.md for more information about why
            // this has to redirect to the main thread.

            asyncCallFromMainThread(std::bind<void, std::function<void(ARG_TYPES...)>, ARG_TYPES...>(
                strongMethod(this->shared_from_this(), &ThreadSafeNotifier::notify), std::forward<ARG_TYPES>(args)...));
        }

      private:
    };
}
