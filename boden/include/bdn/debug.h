#ifndef BDN_DEBUG_H_
#define BDN_DEBUG_H_


namespace bdn
{


/** This is a dummy function that does nothing. It is called by BDN_DEBUG_BREAK when
    the programs wants to break into the debugger. It is useful when BDN_DEBUG_BREAK
    cannot actually break into the debugger on the current platform - then one can set
    a normal breakpoint in this function to achieve something similar.

    Note that debugBreakDummy is actually called on all platforms, even when debug breaking
    is supported.
    */
inline void debugBreakDummy()
{
    // You can set a debug breakpoint here if you want to stop the debugger
    // at points when BDN_DEBUG_BREAK is used.
    bdn::doNothing();
}


}


/** \def BDN_DEBUG_BREAK()

    This is a helper macro that tries to identify whether or not the program is
    being debugged and if it is to break debugger execution (pause the debugger).

    If the program is not being debugged then this macro has no effect.

    IMPORTANT: This macro is implemented on a "best effort" basis. For some compilers and/or platforms
    it will have no effect (i.e. if it cannot detect debuggers, or cannot break their execution).

    Platform notes:

    - Win32: Works with Visual Studio and MingW
    - WinUWP: Works with Visual Studio    
    - Linux: works
    - macOS: works
    - iOS: works
    - other platforms or compilers: Currently the macro cannot halt the debugger. But there is a workaround: BDN_DEBUG_BREAK will call a function
      called debugBreakDummy (defined in debug.h) that one can put a breakpoint in to achieve the same effect.    
*/

#if BDN_PLATFORM_OSX

    namespace bdn
    {
        /** Used internally. Do not call.*/
        bool _isDebuggerActive();
    }

    // The following code snippet based on:
    // http://cocoawithlove.com/2008/03/break-into-debugger.html
    #if defined(__ppc64__) || defined(__ppc__)
        #define BDN_DEBUG_BREAK_IMPL_() \
                if( bdn::_isDebuggerActive() ) { \
                    __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
                    : : : "memory","r0","r3","r4" ); \
                }

    #else
        #define BDN_DEBUG_BREAK_IMPL_() if( bdn::isDebuggerActive() ) {__asm__("int $3\n" : : );}

    #endif

#elif BDN_PLATFORM_WIN32

    namespace bdn
    {
        /** Used internally. Do not call.*/
        bool _isDebuggerActive();
    }

    #if defined(_MSC_VER)
        #define BDN_DEBUG_BREAK_IMPL_() if( bdn::_isDebuggerActive() ) { __debugbreak(); }

    #else
        extern "C" __declspec(dllimport) void __stdcall DebugBreak();
        #define BDN_DEBUG_BREAK_IMPL_() if( bdn::_isDebuggerActive() ) { DebugBreak(); }

    #endif

#elif BDN_PLATFORM_WINUWP

    namespace bdn
    {
        /** Used internally. Do not call.*/
        bool _isDebuggerActive();
    }

    #define BDN_DEBUG_BREAK_IMPL_() if( bdn::_isDebuggerActive() ) { __debugbreak(); }

        
#elif BDN_PLATFORM_ANDROID

    // unfortunately SIGTRAP does not break into the debugger in android studio
    // (at least as of 2016-08-02). Instead the debugger and the program seem
    // to lock up.
    // So right now there is no good way to do a debug break on android. We leave
    // the macro undefined, so that we will get the default implementation that
    // simply calls a dummy function.

#elif BDN_PLATFORM_POSIX

    // Checking wether or not we are being debugged is apparently not that easy on Linux/Unix systems.
    // However, we do not need it since we can raise a SIGTRAP that
    // will stop a debugger and not have any effect on
    // a program that is not being debugged. For that
    // we disable the SIGTRAP for the process,
    // and then raise that signal. If the process is being debugged
    // the signal will be delivered to the debugger instead
	// of this process and cause it to break.
	// If not it is ignored.
	// Note that we simply disable the SIGTRAP signal and leave
	// it at that. It is only used for debugging and if we are
	// not debugged it should not be a problem to ignore it.
	// If we were to restore the old signal action then we'd have
	// to deal with race conditions when multiple threads execute
	// a debugBreak at the same time.

    #include <signal.h>

    // we configure SIGTRAP to be ignored by the runtime lib and then
    // raise SIGTRAP. When a debugger is attached then it will pick up the signal
    // and interrupt the program. When no debugger is used then the
    // signal will be ignored and have no effect.
    #define BDN_DEBUG_BREAK_IMPL_() { \
                                    struct sigaction dbrk_newAction_; \
                                    dbrk_newAction_.sa_handler = SIG_IGN; \
                                    sigemptyset(&dbrk_newAction_.sa_mask); \
                                    dbrk_newAction_.sa_flags = 0; \
                                    if(sigaction(SIGTRAP, &dbrk_newAction_, NULL)==0) \
                                        raise(SIGTRAP); \
                                }

#endif



#ifndef BDN_DEBUG_BREAK_IMPL_

// We do not have a way to break into the debugger for this platform.
// So, do nothing.

#define BDN_DEBUG_BREAK_IMPL_() ;

#endif


#define BDN_DEBUG_BREAK() \
            do \
            { \
                bdn::debugBreakDummy(); \
                BDN_DEBUG_BREAK_IMPL_(); \
            }while(false)

#endif

