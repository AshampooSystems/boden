#ifndef BDN_IPropertyNotifier_H_
#define BDN_IPropertyNotifier_H_

namespace bdn
{
    template<typename PROPERTY_VALUE_TYPE>
    class IPropertyNotifier;
}

#include <bdn/INotifierBase.h>
#include <bdn/IPropertyReadAccessor.h>

namespace bdn
{


/** Interface for notifier objects from properties.
 
    The property notifier handles the special case that a property value changes
    **during** a single notification call (for example, because one subscriber changes
    the value). The notifier ensures that all subscribers always get the current value,
    at the time of the subscriber's notification call.
 
    The biggest difference to generic notifiers (like ISyncNotifier) is that
    the notify function takes a property accessor object.
*/
template<typename PROPERTY_VALUE_TYPE>
class IPropertyNotifier : BDN_IMPLEMENTS INotifierBase< const PROPERTY_VALUE_TYPE& >
{
public:

    
	/** Calls all subscribed functions immediately with the specified arguments.
		All subscribed functions are called before notify returns.
     
        Each subscribed function gets the current value of the property as its parameter.
        If there are multiple subscribers to the notifier and one subscriber changes
        the property's value then the following subscribers will get the new, updated value
        as their parameter.
    */
    virtual void notify( const IPropertyReadAccessor<PROPERTY_VALUE_TYPE>& accessor )=0;
    
};
    
}


#endif


