#ifndef BDN_DanglingFunctionError_H_
#define BDN_DanglingFunctionError_H_

#include <stdexcept>

namespace bdn
{

    /** Thrown when a function is called, but the target object that implements
       the function has already been deleted.

        A common use case when this error is to be expected is with weak methods
        (see weakMethod()). When a weak method is called and its object has been
        deleted then it will throw this exception. Notice that it is not
       considered a programming error when this happens - instead it is
       considered a "normal" condition that can happen during the runtime of the
       application. The caller of the method should handle this error (if he
       can) - usually by removing the method from his internal lists.
        */
    class DanglingFunctionError : public std::bad_function_call
    {
      public:
        DanglingFunctionError(const String &message) : _message(message) {}

        DanglingFunctionError() : _message("A function with a dangling target reference was called.") {}

        const char *what() const noexcept override { return _message.asUtf8Ptr(); }

      private:
        String _message;
    };
}

#endif
