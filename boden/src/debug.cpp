#include <bdn/init.h>
#include <bdn/debug.h>




#ifdef BDN_PLATFORM_OSX
    #include <assert.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/sysctl.h>

    namespace bdn
    {

        // The following function is taken directly from the following technical note:
        // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html

        // Returns true if the current process is being debugged (either
        // running under the debugger or has a debugger attached post facto).
        bool _isDebuggerActive()
        {
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

	        // Call sysctl.

	        size = sizeof(info);
	        if( sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, BDN_NULL, 0) != 0 ) {
		        bdn::cerr() << "\n** Call to sysctl failed - unable to determine if debugger is active **\n" << std::endl;
		        return false;
	        }

	        // We're being debugged if the P_TRACED flag is set.

	        bool active = ( (info.kp_proc.p_flag & P_TRACED) != 0 );

            return active;
        }

    } // namespace bdn

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








