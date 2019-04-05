#pragma once

#include <memory>

namespace bdn
{
    class ApplicationController : public std::enable_shared_from_this<ApplicationController>
    {
      public:
        /** Called when the app launch has begun.

            beginLaunch should set up your user interface here and prepare the
           app to run.

            If the operating system supports automatically restoring the state
           of the app's UI (for example, on iOS) then the restoration has NOT
           yet happened yet when this function is called.

            Note that additional information about how and why the app is run
           (including the commandline arguments) is available in the launchInfo
           parameter.
            */
        virtual void beginLaunch() {}

        /** Called when the app has finished launching, just before it becomes
           active.

            If the operating system supports automatically restoring the state
           of the app's UI (for example, on iOS) then the restoration has
           finished when this function is called.

            This function is intended for making final UI tweaks, just before
           the app becomes active and visible.

            Note that additional information about how and why the app is run
           (including the commandline arguments) is available in the launchInfo
           parameter.
         */
        virtual void finishLaunch() {}

        /** The app has become active and ready for the user to interact with
           it.

            For mobile apps this means that the app is the active app (visible
           in the foreground) or one of the active apps (if the system supports
           multiple apps on the screen at the same time).
            */
        virtual void onActivate() {}

        /** The app will be deactivated or was just deactivated. The user does
           not interact with this app anymore and it is going into the
           background.

            For mobile apps this means that the app was either interrupted (for
           example by a phone call), or the user has switched to another app (or
           the home screen).

            Apps should reduce their activity to a minimum while they are
           inactive. Games might want to pause.

            Note that the app can continue to run while it is in deactivated
           state. But on some systems it will often enter background mode
           shortly afterwards (see onEnterBackgroundMode() )

            */
        virtual void onDeactivate() {}

        /** The application is about to be suspended / frozen.

            An app should store state information for resume or restart here and
            release shared resources. The app should assume that its code
           execution is going to stop while it is suspended and it should not
           expect to get any processing resources during this time. An exception
           are registered background tasks - see below for more information.

            An app should also expect that it might terminate without
           notification while being suspended. So onSuspend should store any
           relevant state information that might be needed for the next launch
           and perform the necessary cleanup.

            On mobile systems onSuspend is typically called at some point after
           the app has been deactivated. The operating system suspends the app
           to save resources.

            onSuspend should return as quickly as possible. On many systems it
           has a limited execution time (typically 1-5 seconds) and the app
           might be terminated if this time is exceeded.

            Some systems offer the option to perform limited, registered
           background tasks while in suspended mode. These can be used to do
           longer cleanup tasks that would take too long for onSuspend. They can
           also be used to perform longer running low-level background
            operations during the suspension. See LimitedBackgroundTask for more
            information.
            */
        virtual void onSuspend() {}

        /** Called when an app is about to exit suspended state
            (see onSuspend() ).

            The app will usually be activated shortly afterwards.

            */
        virtual void onResume() {}

        /** The app is about to be terminated by the operating system (i.e. it
           will be fully unloaded and stop to run).

            \important Not all operating systems support the onTerminate
           notification. On some systems termination will happen without
           notification. You should usually use onSuspend() to store your
           application state and prepare for termination.

            onTerminate should return as quickly as possible. On many systems it
           has a limited execution time (typically 5 seconds) and the app might
           be killed without completing the onTerminate handler if this time is
           exceeded.
            */
        virtual void onTerminate() {}
    };
}
