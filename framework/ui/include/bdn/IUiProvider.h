#ifndef BDN_IUiProvider_H_
#define BDN_IUiProvider_H_

namespace bdn
{
    class IUiProvider;
}

#include <bdn/IViewCore.h>
#include <bdn/View.h>
#include <bdn/ITextUi.h>

namespace bdn
{

    /** Provides the actual implementation for user interface components.

        Most of the UI classes (#Window, #Button, etc) need a platform dependent
        "core object" that creates and manages the actual platform dependent
        UI objects on the screen. These core objects are created by the UI
       provider.

        Custom implementations of a UI provder can be used to provide special UI
       implementations. For example, one could implement a UI provider that
       draws all UI objects manually into a big canvas, instead of using the
       normal operating system components.

        A common use case for a special provider implementation is for unit
       testing. A special mock provider can provide fake objects that simplify
       the testing of a user interface and make it easier to automate such
       tests.
    */
    class IUiProvider : BDN_IMPLEMENTS IBase
    {
      public:
        /** Returns the name of the UI provider. This is intended for logging
         * and diagnostics.*/
        virtual String getName() const = 0;

        /** Create thes core for the specified UI object.

            If the view type is not supported then a
           bdn::ViewCoreTypeNotSupportedError is thrown.

            Initialization of the created core objects
            ------------------------------------------

            In general, the core objects must initialize themselves from the
           properties of the specified outer #View object. I.e. if the View has
           a property like a label then the core object must copy this
            initially.

            The only exception to this is the size and position of the view. The
           View::bounds() property should not be used by the core - it will
           automatically be updated in a subsequent layout cycle after the core
           was created. The core objects should initially have a size and
           position of zero.

            @param coreTypeName the type of the view core. All standard view
           core types start with "bdn." (e.g. bdn.SomeViewName). If you create
           custom views with custom cores then you should start with some
           namespace name followed by a dot and the the name of the core type.
                Usually the namespace prefix should be the name of the C++
           namespace, if that can be considered "sufficiently unique". If there
           is no C++ namespace or if it does not have a unique name then one
           should use a different unique prefix. Java style prefixes based on
           the project's domain name (e.g. "com.mydomain.MyType") are
           recommended in this case.
            @param pView a pointer to the view object. The core object should
           pull all needed data for initialization from this view object (e.g.
           properties like label, visible and the like).

                The view object must have a type that corresponds to the
           requested viewTypeName. I.e. it must be castable to the corresponding
           C++ type.

                IMPORTANT: the core MUST NOT hold a strong reference to the
           outer view object, otherwise a reference cycle and memory leak will
           occur. Note that it is guaranteed that the view object will exist at
           least as long as the core object, so a strong reference is not
           necessary. So you should not store the view pointer in a smart
           pointer (like P<View>). Use weak pointers (like WeakP<View>) instead.
                */
        virtual P<IViewCore> createViewCore(const String &coreTypeName,
                                            View *pView) = 0;

        /** Returns an object that can be used to interact with the user via a
           text interface.

            For Console/Terminal applications this is usually a ITextUi
           implementation that uses the stdio streams (STDOUT / std::cout,
           etc.). One such implementation is bdn::StdioTextUi.

            For other types of applications the text UI implementation will
           often open a graphical window and use normal view components to
           interact with the user (for example with bdn::ViewTextUi ).

            Note that while console / terminal apps are not supported on all
           platforms, a functioning text UI implementation will still provided
           in all cases.

            Also note that the default text UI for each platform will ALWAYS
            also write forward the written text to STDOUT and STDERR, even if a
           graphical window is the primary means of display.

            */
        virtual P<ITextUi> getTextUi() = 0;
    };

    /** Returns the default UI provider for the current platform. Note that this
       is not necessarily the active UI provider that is used by the app. To get
       that use AppControllerBase::getUiProvider().

        For commandline applications (those running exclusively in a Terminal /
       Command window) the default UI provider uses the stdio streams
       (std::cout, etc.) to provide the user interface. In these cases the some
       or all bdn::View types might not be supported by the provider, but text
       UI is always supported (see IUiProvider::getTextUi()).

        This function is thread-safe.
    */
    P<IUiProvider> getDefaultUiProvider();
}

#endif
