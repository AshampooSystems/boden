#pragma once

#include <bdn/ISimpleCallable.h>

#include <list>

namespace bdn::android
{

    /** Executes handlers when the app enters idle state (i.e. when there
     * are not pending events.*/
    class IdleRunner : public Base
    {
      public:
        static IdleRunner &get();

        void callOnceWhenIdle(ISimpleCallable *callable);

        /** Used internally. Notifies the idle runner that an idle phase has
         * begun.*/
        void _beginningIdlePhase();

      private:
        std::list<std::shared_ptr<ISimpleCallable>> _callWhenIdleList;
    };
}
