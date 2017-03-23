#ifndef BDN_INotifierSubControl_H_
#define BDN_INotifierSubControl_H_


namespace bdn
{

/** Can be used to control a notification subscription (see Notifier::subscribe()).*/
class INotifierSubControl : BDN_IMPLEMENTS IBase
{
public:
    /** Unsubscribe. After this function returns the subscribed
        function object will not be called by the Notifier anymore.
            
        This is safe to call even if the Notifier object that returned
        this ISubControl object has already been deleted.            
        */
    virtual void unsubscribe()=0;
};
    
}


#endif


