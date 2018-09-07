#ifndef BDN_WINUWP_Dispatcher_H_
#define BDN_WINUWP_Dispatcher_H_

#include <bdn/IDispatcher.h>

namespace bdn
{
    namespace winuwp
    {

        /** IDispatcher implementation that uses the UWP class
         * Windows.UI.Core.CoreDispatcher.*/
        class Dispatcher : public Base, BDN_IMPLEMENTS IDispatcher
        {
          public:
            Dispatcher(Windows::UI::Core::CoreDispatcher ^ pCoreDispatcher);

            Windows::UI::Core::CoreDispatcher ^ getCoreDispatcher() const
            {
                return _pCoreDispatcher;
            }

            void enqueue(std::function<void()> func,
                         Priority priority = Priority::normal) override;

            void
            enqueueInSeconds(double seconds, std::function<void()> func,
                             Priority priority = Priority::normal) override;

            void createTimer(double intervalSeconds,
                             std::function<bool()> func) override;

            /** Disposes the dispatcher and ensures that all pending items are
             * released (but not executed).*/
            void dispose();

          private:
            void idleHandler(::Windows::UI::Core::IdleDispatchedHandlerArgs ^
                             e);

            template <typename FuncReturnType> struct TimedItem : public Base
            {
                std::function<FuncReturnType()> func;
                IDispatcher::Priority priority;
            };

            class Timer : public Base
            {
              public:
                Timer(Dispatcher *pDispatcher, std::list<P<Timer>>::iterator it,
                      std::function<bool()> func);
                ~Timer();

                void setUwpTimer(::Windows::System::Threading::ThreadPoolTimer ^
                                 pTimer);

                void call();

                void dispose();

              private:
                ::Windows::System::Threading::ThreadPoolTimer ^ _pUwpTimer;

                P<Dispatcher> _pDispatcher;
                std::list<P<Timer>>::iterator _it;

                std::function<bool()> _func;

                bool _disposed = false;

                Mutex _callPendingMutex;
                bool _callPending = false;
            };
            friend class Timer;

            std::list<std::function<void()>> &getQueue(Priority priority);

            void executeItem(Priority priority);

            void disposeQueue(std::list<std::function<void()>> &queue);
            void disposeTimers();

            Windows::UI::Core::CoreDispatcher ^ _pCoreDispatcher;

            Mutex _queueMutex;
            std::list<std::function<void()>> _idleQueue;
            std::list<std::function<void()>> _normalQueue;

            std::list<P<Timer>> _timerList;
        };
    }
}

#endif
