#include <bdn/java/JCharSequence.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace java
    {
        JCharSequence::JCharSequence(const String &str) : JObject(JString(str).getRef_()) {}
    }
}
