
#include <bdn/java/JavaException.h>

#include <bdn/java/wrapper/NativeException.h>

namespace bdn::java
{
    void JavaException::rethrowThrowable(wrapper::Throwable throwable)
    {
        if (throwable.isInstanceOf_(wrapper::NativeException::getStaticClass_())) {
            // this is a C++ exception that was wrapped. Throw that.
            wrapper::NativeException nativeException(throwable.getRef_());

            std::shared_ptr<ExceptionReference> ref = nativeException.getExceptionReference_();

            ref->rethrow();
        } else {
            // a "real" java side exception. Wrap it in JavaException and
            // throw it.
            throw JavaException(throwable);
        }
    }
}
