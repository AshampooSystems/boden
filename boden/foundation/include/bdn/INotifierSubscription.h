#ifndef BDN_INotifierSubscription_H_
#define BDN_INotifierSubscription_H_

namespace bdn
{

    /** Represents a subscription to a notifier object (see
       INotifierBase::subscribe()).

        Note that this interface does not define any methods. No operations are
       currently possible on the subscription objects (beyond what bdn::IBase
       defines) other than passing them to INotifierBase::unsubscribe().
     */
    class INotifierSubscription : BDN_IMPLEMENTS IBase
    {
      public:
    };
}

#endif
