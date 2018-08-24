#ifndef BDN_IWeakReferencable_H_
#define BDN_IWeakReferencable_H_


#include <bdn/IWeakReferenceState.h>

namespace bdn
{


/** INterface for objects that can be references by weak pointers (see WeakP).
*/
class IWeakReferencable : BDN_IMPLEMENTS IBase
{
public:
    

    /** Returns an object that represents the state of weak references.

        This function is thread-safe.
    */
    virtual P<IWeakReferenceState> getWeakReferenceState()=0;


};
    
    
}


#endif


