#include <bdn/init.h>
#include <bdn/java/TypeConversion.h>

#include <bdn/java/JString.h>

namespace bdn
{
namespace java
{

jobject TypeConversionBase_::_createJString(const String& s, std::list<Reference>& createdJavaObjects)
{
    // we have to create a new java-side string object.
    // We must ensure that the java object still exists after we return, so we must add
    // the object to createdJavaObjects.
    
    JString     javaString(s);
    Reference   ref = javaString.getRef_();
    
    createdJavaObjects.push_back( ref );
    
    return ref.getJObject();    
}

String TypeConversionBase_::_getStringFromJava(const Reference& ref)
{
    if(ref.isNull())
        return "";
    else
        return JString(ref).getValue_();
}

}
}


