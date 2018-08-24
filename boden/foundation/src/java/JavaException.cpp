#include <bdn/init.h>
#include <bdn/java/JavaException.h>

#include <bdn/java/JNativeException.h>


namespace bdn
{
namespace java
{


void JavaException::rethrowThrowable(JThrowable throwable)
{
    if(throwable.isInstanceOf_( JNativeException::getStaticClass_() ) )
    {
        // this is a C++ exception that was wrapped. Throw that.
        JNativeException nativeException( throwable.getRef_() );

        P<ExceptionReference> pRef = nativeException.getExceptionReference_();

        pRef->rethrow();
    }
    else
    {
        // a "real" java side exception. Wrap it in JavaException and throw it.
        throw JavaException(throwable);
    }
}

}
}




