#ifndef _BDN_BASE_H_
#define _BDN_BASE_H_

#include <bdn/IBase.h>

namespace bdn
{
    
class Base : virtual public IBase
{
public:
    virtual ~Base()
    {
    }
    
    void addRef() override
    {
    }
    
    void release() override
    {
    }
    
};
    
    
}


#endif


