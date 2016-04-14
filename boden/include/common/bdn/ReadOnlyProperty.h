#ifndef BDN_ReadOnlyProperty_H_
#define BDN_ReadOnlyProperty_H_

#include <bdn/Notifier.h>

namespace bdn
{

/** Base class for properties that can only be read. See Property for
	more information.*/
template<class ValType>
class ReadOnlyProperty : public Base
{
public:
    
	/** Returns the property value.*/
    virtual ValType get() const=0;

	/** Returns a reference to a notifier object that can be used to
		register for change notifications.
		The notification will fire when the property value changes.

		\important The notification call can potentially come from ANY thread.
		The callback function you register here must be able to deal with that.
		As an alternative, you can wrap your callback function with divertToMainThread().
		*/
	virtual Notifier< ReadOnlyProperty >& onChange()=0;


	virtual bool operator==(const ValType& val) const
	{
		return get()==val;
	}

	virtual bool operator!=(const ValType& val) const
	{
		return !operator==(val);
	}


};

}

#endif
