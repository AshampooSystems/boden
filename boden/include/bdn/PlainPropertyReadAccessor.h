#ifndef BDN_PlainPropertyReadAccessor_H_
#define BDN_PlainPropertyReadAccessor_H_

#include <bdn/IPropertyReadAccessor.h>

namespace bdn
{
   

/** Provides access to a property (see \ref properties.md).
 
    Since properties are a simply set of methods, it is sometimes difficult to store them
    or pass a reference to them.
    Accessor objects solve this problem: they representing a specific property of a specific
    owner object and can be passed around and stored like any other object.
 
    PlainPropertyReadAccessor provides read-only access to a property.
    "Plain" means that the accesor object does not do anything to keep the property owner
    alive or to check whether it is still valid. The user is responsible for ensuring that the
    owner is valid.
 
    Note that \ref BDN_PROPERTY_READ_ACCESSOR and makePlainPropertyReadAccessor()
    are two helpers that can be used to conveniently create an instance of PlainPropertyReadAccessor.
    The macro \ref BDN_PROPERTY_READ_ACCESSOR is the most convenient option - it auto-detects all necessary
    template parameters.
 
    Template parameters:
 
    - VALUE_TYPE: the inner value type of the property
    - OWNER_TYPE: the type of the property's owner object
    - GETTER_METHOD_TYPE: the type of the property's getter method. This must be a member function pointer
      to a method of the owner (as indicated by OWNER_TYPE).
    - CHANGED_METHOD_TYPE: the type of the property method that returns the notifier for the "changed" event
      (i.e. the event that fires when the property value is modified). This must be a member function pointer
      to a method of the owner (as indicated by OWNER_TYPE).
 */
template<class VALUE_TYPE, class OWNER_TYPE, typename GETTER_METHOD_TYPE, typename CHANGED_METHOD_TYPE>
class PlainPropertyReadAccessor : public Base, BDN_IMPLEMENTS IPropertyReadAccessor<VALUE_TYPE>
{
public:

    PlainPropertyReadAccessor(OWNER_TYPE* pOwner,
                          GETTER_METHOD_TYPE getterMethod,
                          CHANGED_METHOD_TYPE changedMethod )
    : _pOwner(pOwner)
    , _getterMethod( getterMethod )
    , _changedMethod( changedMethod )
    {
    }
    
    VALUE_TYPE get() const override
    {
        return (_pOwner->*_getterMethod)();
    }
    
    
    IPropertyNotifier<VALUE_TYPE>& changed() const override
    {
        return (_pOwner->*_changedMethod)();
    }
    
protected:
    /** Returns the property owner.*/
    OWNER_TYPE* getOwner() const
    {
        return _pOwner;
    }
    
private:
    OWNER_TYPE*           _pOwner;
    GETTER_METHOD_TYPE    _getterMethod;
    CHANGED_METHOD_TYPE   _changedMethod;
};
    
    
    
/** Helper function to conveniently create a PlainPropertyReadAccessor instance.
 
    The template parameters for OWNER_TYPE, GETTER_METHOD_TYPE and SETTER_METHOD_TYPE
    can be auto-detected and do not need to be specified explicitly.
 
    You can also use the macro \ref BDN_PROPERTY_READ_ACCESSOR, which is even more convenient.
 
    Template parameters:
 
    - VALUE_TYPE: the inner value type of the property
    - OWNER_TYPE: the type of the property's owner object
    - GETTER_METHOD_TYPE: the type of the property's getter method. This must be a member function pointer
      to a method of the owner (as indicated by OWNER_TYPE).
    - CHANGED_METHOD_TYPE: the type of the property method that returns the notifier for the "changed" event
      (i.e. the event that fires when the property value is modified). This must be a member function pointer
      to a method of the owner (as indicated by OWNER_TYPE).
 */
template<class VALUE_TYPE, class OWNER_TYPE, typename GETTER_METHOD_TYPE, typename CHANGED_METHOD_TYPE>
PlainPropertyReadAccessor<VALUE_TYPE, OWNER_TYPE, GETTER_METHOD_TYPE, CHANGED_METHOD_TYPE> makePlainPropertyReadAccessor( OWNER_TYPE* pOwner, GETTER_METHOD_TYPE getterMethod, CHANGED_METHOD_TYPE changedMethod)
{
    return PlainPropertyReadAccessor<VALUE_TYPE, OWNER_TYPE, GETTER_METHOD_TYPE, CHANGED_METHOD_TYPE>( pOwner, getterMethod, changedMethod);
}
    
    
/** Returns a temporary bdn::PlainPropertyReadAccessor object for the corresponding property.
 
    \param owner the owner of the property (the owner object, or a reference to it, not a pointer)
    \param propertyName the name of the property.
 */
#define BDN_PROPERTY_READ_ACCESSOR( owner, propertyName ) \
    bdn::makePlainPropertyReadAccessor< typename std::remove_reference< decltype(owner) >::type :: PropertyValueType_##propertyName>( \
        &(owner), \
        &std::remove_reference< decltype(owner) >::type :: propertyName, \
        &std::remove_reference< decltype(owner) >::type :: propertyName##Changed )

}

#endif
