#ifndef _BDN_APPNOTINSTANTIATED_H_
#define _BDN_APPNOTINSTANTIATED_H_

#include <exception>

namespace bdn
{

class AppNotInstantiatedError : public std::exception
{
public:
    AppNotInstantiatedError()
    {
    }
    
    virtual const char* what() const noexcept override
    {
        return "No app object was instantiated.";
    }
};
    
   
}



#endif


