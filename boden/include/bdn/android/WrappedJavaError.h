#ifndef _BDN_WrappedJavaError_H_
#define _BDN_WrappedJavaError_H_

#include <exception>
#include <string>

#include <jni.h>

namespace bdn
{

class WrappedJavaError : public std::exception
{
public:
    WrappedJavaError(jthrowable exceptionObj)
    {
        _exceptionObj = exceptionObj;
    }

    jthrowable getExceptionObject()
    {
        return _exceptionObj;
    }
    
    virtual const char* what() const noexcept override
    {
        return "Wrapped JAVA error";
    }

protected:
    jthrowable _exceptionObj;
};
    
   
}



#endif


