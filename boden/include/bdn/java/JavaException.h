#ifndef BDN_JAVA_JavaException_H_
#define BDN_JAVA_JavaException_H_

#include <exception>
#include <string>

#include <bdn/java/JThrowable.h>

#include <jni.h>

namespace bdn
{
namespace java
{


/** C++ exception that wraps a Java-side exception object.*/
class JavaException : public std::exception
{
public:
    JavaException( JThrowable& throwable)
    : _throwable( (jthrowable)throwable.getRef_().toStrong().getJObject() )
    {
        _messageUtf8 = (throwable.getCanonicalClassName_() + ": " + throwable.getMessage()).asUtf8();
    }

    JThrowable& getJThrowable_()
    {
        return _throwable;
    }
    
    virtual const char* what() const noexcept override
    {
        return _messageUtf8.c_str();
    }

private:
    JThrowable  _throwable;
    std::string _messageUtf8;
};
    

}
}



#endif


