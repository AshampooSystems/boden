#ifndef BDN_ReadProperty_H_
#define BDN_ReadProperty_H_

#include <bdn/INotifier.h>
#include <bdn/IValueAccessor.h>

namespace bdn
{

/** Base class for properties which only allow the value to be read. See Property for
	more information.
	
	
	*/
template<class ValType>
class ReadProperty : public Base, BDN_IMPLEMENTS IValueAccessor<ValType>
{
public:
    
	/** Returns the property value.*/
    virtual ValType get() const=0;


	/** Allows implicit conversion to the inner value type.*/
	operator ValType() const
	{
		return get();
	}


	/** Returns a reference to a notifier object that can be used to
		register for change notifications.

		The notification will fire when the property value changes. Note that
        the functions that are subscribed to the returned notifier are called
        asynchronously after the change has already happened. These calls always
        happen from the main thread, even if the property was changed in another thread.
        
        The parameter of the notification call is a pointer to a value accessor object.
        This object will return the property's value at the current time.
		*/
	virtual INotifier< P<const IValueAccessor<ValType>> >& onChange() const=0;


	/** Returns a string representation of the property value.
		This usually gives the same result as calling the global
		bdn::toString() function on the property value.*/
	virtual String toString() const=0;

};

}

#endif
