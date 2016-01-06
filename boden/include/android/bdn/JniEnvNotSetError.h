#ifndef _BDN_JniEnvNotSetError_H_
#define _BDN_JniEnvNotSetError_H_

#include <exception>
#include <string>

namespace bdn
{

class JniEnvNotSetError : public std::exception
{
public:
    JniEnvNotSetError()
    {
    }
    
    virtual const char* what() const noexcept override
    {
        return "JNI environment not set.";
    }
};
    
   
}



#endif


