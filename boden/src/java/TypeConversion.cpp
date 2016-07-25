#include <bdn/init.h>
#include <bdn/java/TypeConversion.h>

namespace bdn
{
namespace java
{

jobject TypeConversionBase_::_createLocalStringRef(const String& s)
{
    JavaReference localRef = JString(s).getRef_().toLocal();

    return localRef.detach();
}

String TypeConversionBase_::_getStringFromJava(jobject o)
{
    return JString( Reference(o) ).getValue_();
}

}
}


