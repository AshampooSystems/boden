#ifndef BDN_GTK_MainDispatcher_H_
#define BDN_GTK_MainDispatcher_H_

#include <bdn/IDispatcher.h>

#include <gtk/gtk.h>

namespace bdn
{
    namespace gtk
    {

        /** IDispatcher implementation for the main thread of a GTK application.
         */
        class MainDispatcher : public Base, BDN_IMPLEMENTS IDispatcher
        {
          public:
            MainDispatcher();

            void dispose();

            void enqueue(std::function<void()> func,
                         Priority priority = Priority::normal) override;

            void
            enqueueInSeconds(double seconds, std::function<void()> func,
                             Priority priority = Priority::normal) override;

            void createTimer(double intervalSeconds,
                             std::function<bool()> func) override;

          private:
            static gboolean processNormalItemCallback(gpointer data);
            static gboolean processIdleItemCallback(gpointer data);
            void processItem(std::list<std::function<void()>> &queue);

            struct TimedItem
            {
                TimedItem(const std::function<void()> &func,
                          MainDispatcher *pDispatcher)
                    : func(func), pDispatcher(pDispatcher)
                {}

                std::function<void()> func;
                std::list<TimedItem>::iterator it;

                // strong reference. This is intended
                P<MainDispatcher> pDispatcher;
            };

            void processTimedItem(TimedItem *pItem);
            static gboolean processTimedItemCallback(gpointer data);

            struct Timer : public Base
            {
                Timer(const std::function<bool()> &func,
                      MainDispatcher *pDispatcher)
                    : func(func), pDispatcher(pDispatcher)
                {}

                // strong reference to dispatcher. That is by design
                P<MainDispatcher> pDispatcher;

                std::function<bool()> func;
                std::list<Timer>::iterator it;
                bool disposed = false;
            };

            bool processTimer(Timer *pTimer);
            static gboolean processTimerCallback(gpointer data);

            Mutex _queueMutex;

            std::list<std::function<void()>> _normalQueue;
            std::list<std::function<void()>> _idleQueue;

            std::list<TimedItem> _timedItemList;
            std::list<Timer> _timerList;
        };
    }
}

#endif
