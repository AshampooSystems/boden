
#include <bdn/java/TypeConversion.h>

#include <bdn/java/wrapper/String.h>

namespace bdn::java
{
    jobject TypeConversionBase_::_createJString(const String &s, std::list<Reference> &createdJavaObjects)
    {
        // we have to create a new java-side string object.
        // We must ensure that the java object still exists after we return,
        // so we must add the object to createdJavaObjects.

        wrapper::String javaString(s);
        Reference ref = javaString.getRef_();

        createdJavaObjects.push_back(ref);

        return ref.getJObject();
    }

    String TypeConversionBase_::_getStringFromJava(const Reference &ref)
    {
        if (ref.isNull())
            return "";
        else
            return wrapper::String(ref).getValue_();
    }
}
