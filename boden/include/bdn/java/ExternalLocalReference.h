#ifndef BDN_JAVA_ExternalLocalReference_H_
#define BDN_JAVA_ExternalLocalReference_H_

#include <bdn/java/Reference.h>

namespace bdn
{
namespace java
{

/** Convenience version of #Reference that makes it simpler to
 *  contruct a reference object with type Reference::Type::externalLocal.
 * */
class ExternalLocalReference : public Reference
{
public:
    ExternalLocalReference(jobject localRef)
     : Reference( Type::externalLocal, localRef )
    {
    }
};


}
}


#endif


