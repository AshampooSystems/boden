#include <bdn/init.h>
#include <bdn/entry.h>

#include <bdn/winuwp/platformError.h>
#include <bdn/winuwp/AppRunner.h>

namespace bdn
{
    namespace winuwp
    {
        void _storeParametersForUnhandledException(
            ::Platform::Exception ^ pUwpException,
            bool canKeepRunningAfterException)
        {
            P<AppRunner> pAppRunner = tryCast<AppRunner>(bdn::getAppRunner());
            if (pAppRunner != nullptr)
                pAppRunner->_storeParametersForUnhandledException(
                    pUwpException, canKeepRunningAfterException);
        }
    }

    void platformEntryWrapper(std::function<void()> function,
                              bool canKeepRunningAfterException, void *)
    {
        try {
            /* Sometimes our "entry" functions are executed by the runtime
            because of a call we made to it. If the runtime knows that it was
            called from app code then it will let the exception through to the
            app code, which will then get another chance to handle the
            exception. That is preferable to just assuming that our code was
            called by a "root" routine in the runtime and that there is not more
            chance for normal app code to handle it. So we want to let the
            exception through here. If we were called by root runtime routines
            then that will trigger the app's unhandledException handler to be
            called, which in turn will call the routine in our app runner. So
            that is the same end result that we would get if we called our
            unhandledException handler here. BUT if we were ultimately called by
            app code then the exception is just tunneled through and may be
            handled normally. If not then the exception is propagated up until
            at some point the root is reached and our unhandled exception
            handler is called again.

            There is one drawback to letting the UWP runtime decide: we need to
            convert our C++ exceptions to UWP exceptions. I.e. we lose some
            information there. BUT we can work around that by storing the
            original C++ exception and using that instead of the UWP version
            when our handler is called.
            */
            function();
        }
        catch (::Platform::Exception ^ pException) {
            // let these through. They will cause the UWP unhandled exception
            // handler in our app to be called, which in turn forwards the
            // exception to our own unhandledException handler. */
            bdn::winuwp::_storeParametersForUnhandledException(
                pException, canKeepRunningAfterException);
            throw;
        }
        catch (std::exception &e) {
            // convert C++ exceptions to UWP exceptions. Then we will get the
            // normal UWP handling, which in turn will call our unhandled
            // exception handler, as explained above.
            ::Platform::Exception ^ pException =
                bdn::winuwp::exceptionToPlatformException(e);
            bdn::winuwp::_storeParametersForUnhandledException(
                pException, canKeepRunningAfterException);
            throw pException;
        }
        catch (...) {
            // all other exceptions (not UWP and not std) are converted to a
            // dummy UWP exception.
            ::Platform::Exception ^ pException =
                ::Platform::Exception::CreateException(
                    E_FAIL, ref new ::Platform::String(
                                L"Exception of unknown type encountered."));
            bdn::winuwp::_storeParametersForUnhandledException(
                pException, canKeepRunningAfterException);
            throw pException;
        }
    }
}
