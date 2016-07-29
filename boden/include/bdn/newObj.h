#ifndef BDN_newObj_H_
#define BDN_newObj_H_

#include <utility>

namespace bdn
{

template<class T>
class RawNewAllocator_NonBase_
{
public:
	template<typename... Arguments>
	static T* alloc(Arguments&&... args)
	{
		return new T(std::forward<Arguments>(args)...);
	}
};

template<class T>
class RawNewAllocator_Base_
{
public:
	template<typename... Arguments>
	static T* alloc(Arguments&&... args)
	{
		return new(Base::RawNew::Use) T(std::forward<Arguments>(args)...);
	}
};



template<typename T, typename... Arguments>
T* rawNew(Arguments&&... args)
{
	return std::conditional<
				std::is_base_of<bdn::Base, T>::value,
				RawNewAllocator_Base_<T>,
				RawNewAllocator_NonBase_<T> >::type
					::alloc( std::forward<Arguments>(args)... );
}


template<typename T, typename... Arguments>
P<T> newObj(Arguments&&... args)
{
    return P<T>().attachPtr( new(Base::RawNew::Use) T( std::forward<Arguments>(args)... ) );
}




template<class T>
class DeleteOrReleaseRef_Delete_
{
public:
	static void doIt(T* p)
	{
		delete p;
	}
};

template<class T>
class DeleteOrReleaseRef_Release_
{
public:
	static void doIt(T* p)
	{
		p->releaseRef();
	}
};



/** Deletes or releases the specified object.
	If the type of the object is derived from IBase then
	the reference count is decreased. The object is only deleted if
	the reference count reaches zero.

	If the type of the object is not derived from IBase
	then the object is simply deleted with the delete operator.
	*/
template<class T>
void deleteOrReleaseRef(T* p)
{
	return std::conditional<
				std::is_base_of<bdn::IBase, T>::value,
				DeleteOrReleaseRef_Release_<T>,
				DeleteOrReleaseRef_Delete_<T> >::type
					::doIt( p );
}

}


#endif

