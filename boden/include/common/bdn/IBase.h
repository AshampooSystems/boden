#ifndef BDN_IBase_H_
#define BDN_IBase_H_

#ifdef _MSC_VER
	// disable warning "CLASS : inherits FUNCTION via dominance"
	#pragma warning(disable: 4250)
#endif

#include <typeinfo>

namespace bdn
{


/** Base interface for most classes.*/
class IBase
{
public:
	virtual ~IBase()
	{
	}


	/** Adds 1 to the internal reference count. This should normally NOT be called
		directly. Instead you should use #P smart pointers, which take care of this
		automatically.*/
	virtual void addRef() const=0;


	/** Removes 1 from the internal reference count. When it reaches 0 then
		the object will delete itself.*/
	virtual void releaseRef() const=0;


	/** Called when the object is cast to the specified type.
		Returns true if the cast should be allowed and false otherwise.*/
	virtual bool checkCastAllowed(	const std::type_info& targetType ) const=0;

};
    
    
}


#endif


