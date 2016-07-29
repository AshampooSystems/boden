#ifndef BDN_JAVA_OwnedLocalReference_H_
#define BDN_JAVA_OwnedLocalReference_H_

#include <bdn/java/Reference.h>

namespace bdn
{
namespace java
{

/** Convenience version of #Reference that makes it simpler to
 *  contruct a reference object with type Reference::Type::ownedLocal.
 * */
class OwnedLocalReference : public Reference
{
public:
    OwnedLocalReference(jobject localRef)
     : Reference( Type::ownedLocal, localRef )
    {
    }
};


}
}


#endif


