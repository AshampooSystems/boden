#ifndef _BDN_IDGEN_H_
#define _BDN_IDGEN_H_

#include "Base.h"

namespace bdn
{

class IdGen : public Base
{
public:
    
    static long long newID()
    {
        static long long nextID = 0;
        
        long long id = nextID;
        nextID++;
        
        return id;
    }
};
    
}


#endif


