#include <bdn/java/wrapper/CharSequence.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::java::wrapper
{
    CharSequence::CharSequence(const bdn::String &str) : Object(String(str).getRef_()) {}
}
