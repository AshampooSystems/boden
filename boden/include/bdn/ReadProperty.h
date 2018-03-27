#ifndef BDN_ReadProperty_H_
#define BDN_ReadProperty_H_

#include <bdn/IValueAccessor.h>
#include <bdn/ISyncNotifier.h>

namespace bdn
{

/** Base class for properties which only allow the value to be read. See Property for
	more information.
	
	Property objects are supported by the global function bdn::toString()
	if their inner value (see template parameter ValType) is supported.

	Likewise, Property objects can also be written to standard output streams (std::basic_ostream,
	bdn::TextOutStream, bdn::StringBuffer) with the << operator, if their value
	can be written to such a stream.

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
        immediately after the change has happened. So These calls always
        happen from the main thread, even if the property was changed in another thread.
        
        The parameter of the notification call is a pointer to a value accessor object.
        This object will return the property's value at the current time.
		*/
	virtual ISyncNotifier< P<const IValueAccessor<ValType>> >& onChange() const=0;


};


template< typename CHAR_TYPE, class CHAR_TRAITS, typename PROP_VALUE >
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& operator<<(
	std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
	const ReadProperty<PROP_VALUE>& s )
{
	// note that if there is no << operator for PROP_VALUE then that is not a problem.
	// Since this << operator is a template all that does is remove the operator from the
	// list of possible overloads. So it would be as if there was no << operator for DefaultProperty<PROP_VALUE>,
	// which is exactly what we would want in this case.

	return stream << s.get();
}

}

#endif
