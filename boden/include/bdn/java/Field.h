#ifndef BDN_JAVA_Field_H_
#define BDN_JAVA_Field_H_

#include <bdn/java/FieldId.h>
#include <bdn/java/FieldAccessor.h>
#include <bdn/java/Reference.h>

namespace bdn
{
namespace java
{

/** Represents the field of a Java-side object and enables access to it.
 *
 *  Field instances are light-weight objects that can be returned and passed around by value.
 *  The Field instance encapsulates a reference to the Java-side object, so the object
 *  is kept alive by the field instance itself.
 *
 *  Field instances are self-contained, so it is NOT necessary to keep an accessor object
 *  (subclass of JObject) for the Java-side object alive,
 *  even if the Field instance was returned by the accessor object
 *
 * */
template<class NativeType>
class Field
{
public:
    Field(const Reference& objRef, const FieldId<NativeType>& fieldId)
    : _objRef(objRef)
    , _id(fieldId)
    {

    }

    Field(const Field& o)
    : _objRef( o._objRef )
    , _id( o._id )
    {
    }

    void set(const NativeType& newValue)
    {
        FieldAccessor<NativeType>::setValue( _objRef.getJObject(), _id.getId(), newValue );
    }

    NativeType get() const
    {
        return FieldAccessor<NativeType>::getValue( _objRef.getJObject(), _id.getId() );
    }


    Field& operator=( const NativeType& newValue)
    {
        set(newValue);
        return *this;
    }

    operator NativeType() const
    {
        return get();
    }

private:
    Reference           _objRef;
    FieldId<NativeType> _id;
};


}
}



#endif



