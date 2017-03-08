#ifndef BDN_MethodP_H_
#define BDN_MethodP_H_

#include <utility>


namespace bdn
{


/** Wraps an object method and allows it to be called like
    a normal function.
    Holds a strong reference to the object that the method is called
    on (i.e. that object is kept alive as long as the MethodP instance
    exists).
    
    The easiest way to create MethodP instances is with
    makeMethodP()
    
    A MethodP can be null (see isNull()). Calling a null MethodP results
    in a ProgrammingError exception.
 
    */
template<class ObjectType, class MethodType>
class MethodP
{
public:
    MethodP()
    : _method(nullptr)
    {
    }
    
    MethodP(ObjectType* pObject, MethodType method)
        : _pObject(pObject)
        , _method(method)
    {
    }
    
    template <class... ArgTypes>
    typename std::result_of< MethodType(ObjectType*, ArgTypes...)>::type operator() (ArgTypes&&... args) const
    {
        if(isNull())
            throw ProgrammingError("Tried to call null method.");
    
        return (*(_pObject.getPtr()).*_method)( std::forward<ArgTypes>(args)... );
    }
    
    
    /** Returns true if the method pointer is null.
        Null methods will throw an exception when they are called.*/
    bool isNull() const
    {
        return (_pObject==nullptr || _method==nullptr);
    }
    
    
    /** Same as isNull().*/
    bool operator==(std::nullptr_t) const
    {
        return isNull();
    }
    
    /** Same as ! isNull().*/
    bool operator!=(std::nullptr_t) const
    {
        return !isNull();
    }

    
private:
    P<ObjectType>   _pObject;
    MethodType      _method;
};



template<class ObjectType, typename ReturnType>
MethodP<ObjectType, ReturnType (ObjectType::*)> makeMethodP( ObjectType* pObjectType, ReturnType ObjectType::* method )
{
    return MethodP<ObjectType, ReturnType (ObjectType::*)>(pObjectType, method);
}

template<class ObjectType, typename ReturnType>
MethodP<ObjectType, ReturnType (ObjectType::*)> makeMethodP( const P<ObjectType>& pObjectType, ReturnType ObjectType::* method )
{
    return MethodP<ObjectType, ReturnType (ObjectType::*)>(pObjectType.getPtr(), method);
}


}


#endif
