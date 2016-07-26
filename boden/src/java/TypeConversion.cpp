#include <bdn/init.h>
#include <bdn/java/TypeConversion.h>

#include <bdn/java/JString.h>

namespace bdn
{
namespace java
{

jobject TypeConversionBase_::_createLocalStringRef(const String& s)
{
    Reference localRef = JString(s).getRef_().toLocal();

    return localRef.detach();
}

String TypeConversionBase_::_getStringFromJava(const Reference& ref)
{
    return JString(ref).getValue_();
}

}
}


