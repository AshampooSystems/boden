#include <bdn/init.h>
#include <bdn/mainThread.h>

namespace bdn
{

void CallFromMainThreadBase_::dispatch()
{
    // inc refcount so that object is kept alive until the call happens.
    addRef();


}



}






