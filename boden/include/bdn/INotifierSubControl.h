#ifndef BDN_INotifierSubControl_H_
#define BDN_INotifierSubControl_H_


namespace bdn
{

/** Can be used to control a notification subscription (see INotifierBase::subscribe()).*/
class INotifierSubControl : BDN_IMPLEMENTS IBase
{
public:
    /** Unsubscribe the notification function.

        If this is called from the main thread then the notification function will not be called
        again after unsubscribe returns.

        BUT if this is called from another thread then there are cases when the unsubscribed function
        may be called even after unsubscribe returns. This can happen if a notification is in progress
        when unsubscribe is called.

        unsubscribe() is safe to call even if the INotifier object that returned
        this ISubControl object has already been deleted. In that case it will simply have no effect.
        */
    virtual void unsubscribe()=0;
};
    
}


#endif


