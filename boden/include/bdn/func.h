#ifndef BDN_method_H_
#define BDN_method_H_

#include <utility>
#include <functional>


namespace bdn
{


/** Internal class. Do not use directly.
    */
template<class ObjectType, class MethodType>
class BoundMethod_
{
public:
    BoundMethod_()
    : _method(nullptr)
    {
    }
    
    BoundMethod_(ObjectType* pObject, MethodType method)
        : _pObject(pObject)
        , _method(method)
    {
    }
    
    template <class... ArgTypes>
    typename std::result_of< MethodType(ObjectType*, ArgTypes...)>::type operator() (ArgTypes&&... args) const
    {
        if(!isValid())
            throw std::bad_function_call();
    
        return (*(_pObject.getPtr()).*_method)( std::forward<ArgTypes>(args)... );
    }
    
    
    /** Returns true if the method pointer is valid.
        Invalid methods will throw an exception when they are called.*/
    bool isValid() const
    {
        return (_pObject!=nullptr && _method!=nullptr);
    }
    
    
    /** Returns true if the method is invalid (see isValid()).*/
    bool operator==(std::nullptr_t) const
    {
        return !isValid();
    }
    
    /** Returns true if the method is valid (see isValid()).*/
    bool operator!=(std::nullptr_t) const
    {
        return isValid();
    }
    
    /** Returns true if the method is valid.*/
    explicit operator bool() const
    {
        return isValid();
    }

    
private:
    P<ObjectType>   _pObject;
    MethodType      _method;
};




/** Internal class. Do not use directly.
 */
template<class ObjectType, class MethodType>
class WeakBoundMethod_
{
public:
    WeakBoundMethod_()
    : _method(nullptr)
    {
    }
    
    WeakBoundMethod_(ObjectType* pObject, MethodType method)
    : _pObject(pObject)
    , _method(method)
    {
    }
    
    template <class... ArgTypes>
    typename std::result_of< MethodType(ObjectType*, ArgTypes...)>::type operator() (ArgTypes&&... args) const
    {
        if(!isValid())
            throw std::bad_function_call();
        
        return ((*_pObject).*_method)( std::forward<ArgTypes>(args)... );
    }
    
    
    /** Returns true if the method pointer is valid.
     Invalid methods will throw an exception when they are called.*/
    bool isValid() const
    {
        return (_pObject!=nullptr && _method!=nullptr);
    }
    
    
    /** Returns true if the method is invalid (see isValid()).*/
    bool operator==(std::nullptr_t) const
    {
        return !isValid();
    }
    
    /** Returns true if the method is valid (see isValid()).*/
    bool operator!=(std::nullptr_t) const
    {
        return isValid();
    }
    
    /** Returns true if the method is valid.*/
    explicit operator bool() const
    {
        return isValid();
    }
    
    
private:
    ObjectType*   _pObject;
    MethodType    _method;
};


/**  Packages a method together with its object pointer and allows it to be called
     like a global function (without providing the object pointer at the point of the call).
     
     The wrapped method object holds a strong reference to the method's object. So the object is kept
     alive as long as the returned method object exists.
     
     You can also use this with virtual methods. They work as expected, just as if the
     method was called normally.
     
     This function only works with methods of classes that implement IBase (which is automatically the case for
     all classes that derive from Base, i.e. for most classes in this framework). A similar function
     that also works for other classes is weakMethod().
 
     Example:
     
     \code
     
     class MyClass : public Base
     {
     public:
     
        int hello(String s);
     };
     
     P<MyClass> pObject = newObj<MyClass>();
     
     std::function<int(String)> method = bindMethod(pObject, &MyClass::hello);
     
     // the method can now be called without the need to provide pObject.
     int returnValue = method("abc");
     
     // pObject can also be released - the method object will keep the object alive automatically.
     pObject = nullptr;
     returnValue = method("abc");
     
     \endcode
 
     */
template<class ObjectType, typename FuncType>
std::function<FuncType> bindMethod( ObjectType* pObject, FuncType ObjectType::* method )
{
    if(pObject==nullptr || method==nullptr)
        return std::function<FuncType>();
    else
        return BoundMethod_<ObjectType, FuncType (ObjectType::*)>(pObject, method);
}

template<class ObjectType, typename FuncType>
std::function<FuncType> bindMethod( const P<ObjectType>& pObject, FuncType ObjectType::* method )
{
    if(pObject==nullptr || method==nullptr)
        return std::function<FuncType>();
    else
        return BoundMethod_<ObjectType, FuncType (ObjectType::*)>(pObject.getPtr(), method);
}



/** This is similar to bindMethod(), except that the returned callable will not hold a strong
    reference to the object that the method is working on. So the returned callable will not
    keep that object alive.
    
    Use this carefully. If the object is deleted and the returned callable is called
    afterwards then this will likely cause a crash.
    
    weakBindMethod can can be used with any class - the class does not have to implement
    IBase as with bindMethod().
 
    */
template<class ObjectType, typename FuncType>
std::function<FuncType> weakBindMethod( ObjectType* pObject, FuncType ObjectType::* method )
{
    if(pObject==nullptr || method==nullptr)
        return std::function<FuncType>();
    else
        return WeakBoundMethod_<ObjectType, FuncType (ObjectType::*)>(pObject, method);
}

template<class ObjectType, typename FuncType>
std::function<FuncType> weakBindMethod( const P<ObjectType>& pObject, FuncType ObjectType::* method )
{
    if(pObject==nullptr || method==nullptr)
        return std::function<FuncType>();
    else
        return WeakBoundMethod_<ObjectType, FuncType (ObjectType::*)>(pObject.getPtr(), method);
}





}


#endif
