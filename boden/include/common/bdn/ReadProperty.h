#ifndef BDN_ReadProperty_H_
#define BDN_ReadProperty_H_

#include <bdn/Notifier.h>

namespace bdn
{

/** Base class for properties which only allow the value to be read. See Property for
	more information.
	
	
	*/
template<class ValType>
class ReadProperty : public Base
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
		The notification will fire when the property value changes.

		\important The notification call can potentially come from ANY thread.
		The callback function you register here must be able to deal with that.
		As an alternative, you can wrap your callback function with divertToMainThread().
		*/
	virtual Notifier< const ReadProperty<ValType>& >& onChange()=0;


};

}

#endif
