#include <bdn/init.h>
#include <bdn/debug.h>

#if BDN_PLATFORM_ANDROID
#include <android/log.h>
#endif

#include <bdn/IAppRunner.h>
#include <bdn/Thread.h>


#ifdef BDN_PLATFORM_OSX
    #include <assert.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/sysctl.h>

    namespace bdn
    {

        bool _isDebuggerActive()
        {
            // Source:
            // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html

	        int                 mib[4];
	        struct kinfo_proc   info;
	        size_t              size;

	        // Initialize the flags so that, if sysctl fails for some bizarre
	        // reason, we get a predictable result.

	        info.kp_proc.p_flag = 0;

	        // Initialize mib, which tells sysctl the info we want, in this case
	        // we're looking for information about a specific process ID.

	        mib[0] = CTL_KERN;
	        mib[1] = KERN_PROC;
	        mib[2] = KERN_PROC_PID;
	        mib[3] = getpid();

	        size = sizeof(info);
	        if( sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0) != 0 )
            {
		        // unable to determine if debugger is running. Assuming that it is not.
		        return false;
	        }

	        // We're being debugged if the P_TRACED flag is set.

	        bool active = ( (info.kp_proc.p_flag & P_TRACED) != 0 );

            return active;
        }

    }

#elif BDN_PLATFORM_WIN32 || BDN_PLATFORM_WINUWP

    #include <Windows.h>

    namespace bdn
    {

        bool _isDebuggerActive()
        {
	        return ::IsDebuggerPresent() != 0;
        }

    }

#endif


#if BDN_PLATFORM_WIN32
    
    namespace bdn
    {
        void debuggerPrint(const String& text)
        {
            OutputDebugStringW( (text+"\n").asWidePtr() );
        }
        
        bool debuggerPrintGoesToStdErr()
        {
            return false;
        }
    }

#elif BDN_PLATFORM_WINUWP
    
    namespace bdn
    {
        void debuggerPrint(const String& text)
        {
            OutputDebugStringW( (text+"\n").asWidePtr() );
        }
        
        bool debuggerPrintGoesToStdErr()
        {
            return false;
        }
    }

#elif BDN_PLATFORM_ANDROID


namespace bdn
{
    void debuggerPrint(const String& text)
    {
        __android_log_write(ANDROID_LOG_DEBUG, "boden", text.asUtf8Ptr() );
    }
    
    bool debuggerPrintGoesToStdErr()
    {
        return false;
    }
}


#elif BDN_PLATFORM_IOS

namespace bdn
{
    void debuggerPrint(const String& text)
    {
        // stderr is connected to the debugger.
        std::cerr << text.asUtf8() << std::endl;
    }
    
    bool debuggerPrintGoesToStdErr()
    {
        return true;
    }
}

#elif BDN_PLATFORM_WEBEMS

namespace bdn
{
    void debuggerPrint(const String& text)
    {
        // stderr might be connected to emrun, so the person
        // debugging might see this.
        std::cerr << text.asUtf8() << std::endl;
    }
    
    bool debuggerPrintGoesToStdErr()
    {
        return true;
    }
}


#elif BDN_PLATFORM_OSX || BDN_PLATFORM_LINUX

namespace bdn
{
    void debuggerPrint(const String& text)
    {
        // If we have a UI app then we can output the debug text to stderr.
        // The debugger will usually pick that up.
        // For commandline apps we must not do that, since stderr may actually
        // be used for user interaction there.
        if( ! getAppRunner()->isCommandLineApp() )
            std::cerr << text.asUtf8() << std::endl;
    }
    
    bool debuggerPrintGoesToStdErr()
    {
        return ! getAppRunner()->isCommandLineApp();
    }
}

#else

    namespace bdn
    {
        void debuggerPrint(const String& text)
        {
            // do nothing
        }
        
        bool debuggerPrintGoesToStdErr()
        {
            return false;
        }
    }

#endif







