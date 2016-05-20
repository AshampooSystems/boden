#ifndef BDN_cast_H_
#define BDN_cast_H_

#include <bdn/CastError.h>

namespace bdn
{
	
	
/** Tries to cast the specified object pointer to a DestType pointer (DestType is the template parameter).
	Returns nullptr if the object does not have a compatible type.

	If pObject is nullptr then tryCast also returns nullptr.
	
	*/
template<class DestType>
DestType* tryCast(IBase* pObject)
{
	return dynamic_cast<DestType*>(pObject);
}

/** Tries to cast the specified object pointer to a DestType pointer (DestType is the template parameter).
	Returns nullptr if the object does not have a compatible type.

	If pObject is nullptr then tryCast also returns nullptr.
	
	*/
template<class DestType>
const DestType* tryCast(const IBase* pObject)
{
	return dynamic_cast<const DestType*>(pObject);
}


/** Casts specified object pointer to a DestType pointer (DestType is the template parameter).
	Throws a CastError exception if the object does not have a compatible type.

	If pObject is nullptr then cast simply returns nullptr - no matter what the type of the input
	pointer was.
	
	*/
template<class DestType>
DestType* cast(IBase* pObject)
{
	DestType* pResult = tryCast<DestType>(pObject);
	if(pResult==nullptr && pObject!=nullptr)
		throw CastError( typeid(*pObject), typeid(DestType) );

	return pResult;
}


/** Casts specified object pointer to a DestType pointer (DestType is the template parameter).
	Throws a CastError exception if the object does not have a compatible type.	

	If pObject is nullptr then cast simply returns nullptr - no matter what the type of the input
	pointer was.

	*/
template<class DestType>
const DestType* cast(const IBase* pObject)
{
	const DestType* pResult = tryCast<DestType>(pObject);
	if(pResult==nullptr && pObject!=nullptr)
		throw CastError( typeid(*pObject), typeid(DestType) );

	return pResult;
}

}

#endif