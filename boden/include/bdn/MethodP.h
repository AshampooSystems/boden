#ifndef BDN_MethodP_H_
#define BDN_MethodP_H_

#include <utility>
#include <functional>


namespace bdn
{


/** Internal class. Do not use directly.

    Wraps an object method and allows it to be called like
    a normal function.
    Holds a strong reference to the object that the method is called
    on (i.e. that object is kept alive as long as the MethodP instance
    exists).
    
    The easiest way to create MethodP instances is with
    makeMethodP()
    
    A MethodP objects can be null (see isNull()). Calling a null MethodP results
    in a std::bad_function_call exception.
    
 
 
    */
template<class ObjectType, class MethodType>
class MethodP_
{
public:
    MethodP_()
    : _method(nullptr)
    {
    }
    
    MethodP_(ObjectType* pObject, MethodType method)
        : _pObject(pObject)
        , _method(method)
    {
    }
    
    template <class... ArgTypes>
    typename std::result_of< MethodType(ObjectType*, ArgTypes...)>::type operator() (ArgTypes&&... args) const
    {
        if(isNull())
            throw std::bad_function_call();
    
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


/**  Wraps an object method and allows it to be called like
     a normal function (without the need to provide the pointer to the class instance
     that the method works on).
 
     A pointer to the object is stored internally. The wrapped object
     holds a strong reference to that object (i.e. that object is kept alive as long
     as the wrapped method instance exists).
     
     The returned object can be implicitly converted to a std::function. In fact, that
     is the preferred way to store the returned method wrapper.
     
     Example:
     
     \code
     
     class MyClass : public Base
     {
     public:
     
        int hello(String s);
     };
     
     P<MyClass> pObject = newObj<MyClass>();
     
     std::function<int(String)> method = makeMethodP(pObject, &MyClass::hello);
     
     // method can now be called without the need to provide pObject.
     int returnValue = method("abc");
     
     // pObject can also be released - method will keep the object alive automatically.
     pObject = nullptr;
     returnValue = method("abc");
     
     \endcode
 
     */
template<class ObjectType, typename ReturnType>
MethodP_<ObjectType, ReturnType (ObjectType::*)> makeMethodP( ObjectType* pObject, ReturnType ObjectType::* method )
{
    return MethodP_<ObjectType, ReturnType (ObjectType::*)>(pObject, method);
}

template<class ObjectType, typename ReturnType>
MethodP_<ObjectType, ReturnType (ObjectType::*)> makeMethodP( const P<ObjectType>& pObject, ReturnType ObjectType::* method )
{
    return MethodP_<ObjectType, ReturnType (ObjectType::*)>(pObject.getPtr(), method);
}





}


#endif
