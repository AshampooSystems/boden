#include <bdn/java/wrapper/CharSequence.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::java::wrapper
{
    CharSequence::CharSequence(const std::string &str) : Object(String(str).getRef_()) {}
}
