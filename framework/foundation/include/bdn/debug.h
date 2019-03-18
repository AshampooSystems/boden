#pragma once

#include <bdn/String.h>

namespace bdn
{
    /** Prints a line of text to an output channel to that it can most easily be
       seen in an active debugger, if the program is being debugged.

        Where exactly the text ends up depends on the target platform.
        On some platforms special functions for the debugger are used, on some
       platforms the text ends up in the debug log, on some platforms it might
       go to STDERR (if that is safe).

        Note that you can use debuggerPrintGoesToStdErr() to find out whether or
       not the text is written to stderr in the current app.

        Each debuggerPrint text entry is separated automatically from other
       debuggerPrint text entries, so that the entries can be distringuished
       from each other. For example, if the output channel is simply a text
       stream then a linebreak is automatically added after each entry.

        Some platforms might not have any way for a program to pass text to a
       debugger. On those platforms this function might not do anything.

        The macro BDN_DEBUG_PRINT does the same thing as this function, but it
       is automatically removed from the program in release build.

        Platform note
        -------------------

        The following list contains information on how this function is
       CURRENTLY implemented on various platforms. You should not depend on this
       - the implementation is subject to change!

        - mac commandline apps: do nothing
        - mac graphial apps: print to stderr
        - ios: print to stderr
        - android: print to android log

        */
    void debuggerPrint(const String &message);

    /** Returns true if the debuggerPrint functions forwards the data
        to STDERR.*/
    bool debuggerPrintGoesToStdErr();
}

/** \def BDN_DEBUGGER_PRINT
 Same as bdn::debuggerPrint(), except that the whole statement is automatically
 removed in release builds.
 */

// XXX need to detect debug builds here
#define BDN_DEBUGGER_PRINT(text) bdn::debuggerPrint(text)

/** \def BDN_DEBUG_BREAK()

    This is a helper macro that tries to identify whether or not the program is
    being debugged and if it is to break debugger execution (pause the
   debugger).

    If the program is not being debugged then this macro has no effect.

    IMPORTANT: This macro is implemented on a "best effort" basis. For some
   compilers and/or platforms it will have no effect (i.e. if it cannot detect
   debuggers, or cannot break their execution).

    Platform notes:

    - Win32: Works with Visual Studio and MingW
    - WinUWP: Works with Visual Studio
    - Linux: works
    - macOS: works
    - iOS: works
    - other platforms or compilers: Currently the macro cannot halt the
   debugger. But there is a workaround: BDN_DEBUG_BREAK will call a function
      called debugBreakDummy (defined in debug.h) that one can put a breakpoint
   in to achieve the same effect.
*/

/** \def BDN_DEBUGGER_PRINT
 Same as bdn::debuggerPrint(), except that the whole statement is automatically
 removed in release builds.
 */

// XXX need to detect debug builds here
#define BDN_DEBUGGER_PRINT(text) bdn::debuggerPrint(text)
