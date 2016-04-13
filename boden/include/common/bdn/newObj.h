#ifndef BDN_newObj_H_
#define BDN_newObj_H_


namespace bdn
{

template<typename T, typename... Arguments>
P<T> newObj(Arguments&&... args)
{
    return P<T>().attach( new(Base::RawNew::Use) T(std::forward<Arguments>(args)...) );
}


}


#endif

