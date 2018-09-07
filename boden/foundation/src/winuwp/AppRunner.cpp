#include <bdn/init.h>
#include <bdn/winuwp/AppRunner.h>

#include <bdn/entry.h>

#include <bdn/win32/ThreadLocalStorageManager.h>
#include <bdn/win32/hresultError.h>

namespace bdn
{
    namespace win32
    {

        BDN_SAFE_STATIC_IMPL(ThreadLocalStorageManager,
                             ThreadLocalStorageManager::get);
    }
}

namespace bdn
{
    namespace winuwp
    {

        BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(
            AppRunner::StoredUnhandledException,
            AppRunner::getThreadLocalStoredUnhandledException)

        void _storeParametersForUnhandledException(
            ::Platform::Exception ^ pUwpException,
            bool canKeepRunningAfterException)
        {
            // call the App runners method.

            P<AppRunner> pAppRunner = tryCast<AppRunner>(bdn::getAppRunner());
            if (pAppRunner != nullptr)
                pAppRunner->_storeParametersForUnhandledException(
                    pUwpException, canKeepRunningAfterException);
        }

        ref class App sealed
            : public ::Windows::UI::Xaml::Application,
              public ::Windows::UI::Xaml::Markup::IXamlMetadataProvider
        {
            internal : App(AppRunner *pAppRunner)
            {
                _pAppRunner = pAppRunner;

                InitializeComponent();

                Suspending += ref new Windows::UI::Xaml::SuspendingEventHandler(
                    this, &App::suspending);

                Resuming += ref new Windows::Foundation::EventHandler<Object ^>(
                    this, &App::resuming);
            }

          public:
            virtual ::Windows::UI::Xaml::Markup::IXamlType ^
                GetXamlType(::Windows::UI::Xaml::Interop::TypeName type) {
                    BDN_ENTRY_BEGIN;

                    return nullptr; // return
                                    // getXamlTypeInfoProvider()->GetXamlTypeByType(type);

                    BDN_ENTRY_END(false);

                    return nullptr;
                }

                virtual ::Windows::UI::Xaml::Markup::IXamlType
                ^
                GetXamlType(::Platform::String ^ fullName) {
                    BDN_ENTRY_BEGIN;

                    return nullptr; // return
                                    // getXamlTypeInfoProvider()->GetXamlTypeByName(fullName);

                    BDN_ENTRY_END(false);

                    return nullptr;
                }

                virtual ::Platform::Array<
                    ::Windows::UI::Xaml::Markup::XmlnsDefinition> ^
                GetXmlnsDefinitions() {
                    BDN_ENTRY_BEGIN;

                    return nullptr; // return ref new
                                    // ::Platform::Array<::Windows::UI::Xaml::Markup::XmlnsDefinition>(0);

                    BDN_ENTRY_END(false);

                    return nullptr;
                }

                protected :

                virtual void OnLaunched(Windows::ApplicationModel::Activation::
                                            LaunchActivatedEventArgs ^
                                        pArgs) override
            {
                BDN_ENTRY_BEGIN;

                _pAppRunner->launch();

                BDN_ENTRY_END(true);
            }

            void unhandledUwpException(
                ::Platform::Object ^ pSender,
                ::Windows::UI::Xaml::UnhandledExceptionEventArgs ^ pArgs)
            {
                BDN_ENTRY_BEGIN;

                if (_pAppRunner != nullptr)
                    _pAppRunner->_unhandledUwpException(pSender, pArgs);

                BDN_ENTRY_END(true);
            }

            void InitializeComponent()
            {
                BDN_ENTRY_BEGIN;

                UnhandledException +=
                    ref new ::Windows::UI::Xaml::UnhandledExceptionEventHandler(
                        this, &App::unhandledUwpException);

                BDN_ENTRY_END(false);
            }

            void suspending(Platform::Object ^ pSender,
                            Windows::ApplicationModel::SuspendingEventArgs ^
                                pArgs)
            {
                BDN_ENTRY_BEGIN;

                AppControllerBase::get()->onSuspend();

                BDN_ENTRY_END(true);
            }

            void resuming(Platform::Object ^ pSender, Platform::Object ^ pArgs)
            {
                BDN_ENTRY_BEGIN

                AppControllerBase::get()->onResume();

                BDN_ENTRY_END(true);
            }

            internal : P<AppRunner> _pAppRunner;
        };

        AppLaunchInfo AppRunner::makeAppLaunchInfo(
            Platform::Array<Platform::String ^> ^ args)
        {
            AppLaunchInfo launchInfo;

            // there is no way to pass commandline arguments to a universal
            // windows app. The args parameter does not contain commandline
            // arguments.

            // However, we must add the the dummy commandline argument for the
            // program name.

            std::vector<String> dummyArgs;
            dummyArgs.push_back("");
            launchInfo.setArguments(dummyArgs);

            return launchInfo;
        }

        AppRunner::AppRunner(
            std::function<P<AppControllerBase>()> appControllerCreator,
            Platform::Array<Platform::String ^> ^ args)
            : AppRunnerBase(appControllerCreator, makeAppLaunchInfo(args))
        {}

        bool AppRunner::isCommandLineApp() const
        {
            // UWP does not support commandline apps.
            return false;
        }

        void AppRunner::prepareLaunch()
        {
            AppRunnerBase::prepareLaunch();

            _pMainDispatcher = newObj<bdn::winuwp::Dispatcher>(
                Windows::ApplicationModel::Core::CoreApplication::MainView
                    ->CoreWindow->Dispatcher);
        }

        int AppRunner::entry()
        {
            P<AppRunner> pThis = this;

            Windows::UI::Xaml::Application::Start(
                ref new Windows::UI::Xaml::ApplicationInitializationCallback(
                    [pThis](Windows::UI::Xaml::
                                ApplicationInitializationCallbackParams ^
                            pParams) {
                        BDN_ENTRY_BEGIN;

                        ref new App(pThis);

                        BDN_ENTRY_END(false);
                    }));

            return 0;
        }

        P<IDispatcher> AppRunner::getMainDispatcher()
        {
            return _pMainDispatcher;
        }

        void AppRunner::initiateExitIfPossible(int exitCode)
        {
            // UWP apps must not exit themselves. So, ignore this and do
            // nothing.
            bdn::doNothing();
        }

        void AppRunner::disposeMainDispatcher() { _pMainDispatcher->dispose(); }

        void AppRunner::_storeParametersForUnhandledException(
            ::Platform::Exception ^ pUwpException,
            bool canKeepRunningAfterException)
        {
            // this is called by the BDN_ENTRY_END macro when an unhandled
            // exception is let through to the UWP runtime. We expect that the
            // app's unhandled exception handler is called, which in turn will
            // call our own unhandled exception handler. See comment in
            // BDN_ENTRY_END for more information.

            // What is important here is that we store the original C++
            // exception here and use that if unhandledException is called. That
            // way we do not have to use the derived UWP exception and do not
            // lose information.

            StoredUnhandledException &stored =
                getThreadLocalStoredUnhandledException();

            stored.exceptionPtr = std::current_exception();
            stored.canKeepRunning = canKeepRunningAfterException;

            // store the error code and message of the UWP exception so that we
            // can verify that the unhandled exception call really is the same
            // one we expect here.
            stored.hresult = pUwpException->HResult;
            stored.message = pUwpException->Message;
        }

        void AppRunner::_unhandledUwpException(
            ::Platform::Object ^ pSender,
            ::Windows::UI::Xaml::UnhandledExceptionEventArgs ^ pArgs)
        {
            bool canKeepRunning = false;

            StoredUnhandledException &stored =
                getThreadLocalStoredUnhandledException();

            try {
                // rethrow the exception so that it becomes the active one.

                HRESULT hresult = pArgs->Exception.Value;

                if (stored.exceptionPtr && hresult == stored.hresult &&
                    pArgs->Message == stored.message) {
                    canKeepRunning = stored.canKeepRunning;

                    try {
                        // this is the unhandled exception we expected. So
                        // instead of the UWP exception we get here we use the
                        // C++ exception we stored earlier (see
                        // storeParametersForUnhandledException).
                        std::rethrow_exception(stored.exceptionPtr);
                    }
                    catch (::Platform::Exception ^ pException) {
                        // we can get a UWP exception here, if the active
                        // exception when storeUnhandledExceptionParams was
                        // called was a stored a UWP exception. These are only a
                        // HRESULT code and error message, so there is not much
                        // reason to keep it as a UWP exception. In fact,
                        // platform independent app code will be more likely to
                        // handle it if it is a system_error instead (since it
                        // can then actually access the HRESULT code and
                        // message). So convert it into that...
                        String message(pException->Message->Data());

                        throw bdn::win32::hresultToSystemError(
                            pException->HResult, message);
                    }
                } else {
                    // generate a C++ exception that corresponds to the UWP
                    // exception
                    String message(pArgs->Message->Data());

                    throw bdn::win32::hresultToSystemError(hresult, message);
                }
            }
            catch (...) {
                if (unhandledException(canKeepRunning)) {
                    // ignore this exception.
                    pArgs->Handled = true;
                }
            }

            // note that if we do not set Handled=true then the app will
            // terminate. So nothing else to do here.

            // release any stored exception pointers here. If we did match then
            // we used it. If we did not then no additional call will come for
            // that exception.
            stored.exceptionPtr = std::exception_ptr();
        }
    }
}
