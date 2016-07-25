#ifndef BDN_JAVA_LocalReference_H_
#define BDN_JAVA_LocalReference_H_

#include <bdn/java/Reference.h>

namespace bdn
{
namespace java
{

/** Convenience version of #Reference that makes it simpler to
 *  wrap a local reference jobject in a Reference object.
 * */
class LocalReference : public Reference
{
public:
    LocalReference(jobject localRef)
     : Reference( Type::local, localRef )
    {
    }
};


}
}


#endif


