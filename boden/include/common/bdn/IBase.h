#ifndef _BDN_IBASE_H_
#define _BDN_IBASE_H_

#include <bdn/IBase.h>

#ifdef _MSC_VER
#pragma warning(disable: 4250)
#endif

namespace bdn
{
    
class IBase
{
public:
    
    virtual void addRef()=0;
    virtual void release()=0;
    
};
    
    
}


#endif


