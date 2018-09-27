#pragma once

#include <bdn/Array.h>
#include <bdn/String.h>

namespace bdn
{

    /** Contains information for the app launch, like commandline arguments
        etc.*/
    class AppLaunchInfo : public Base
    {
      public:
        AppLaunchInfo() = default;

        /** Returns the app's commandline arguments.

            The first item in the argument list is a representation of the
           program name and/or path. You should not depend on this first entry,
           though. It is highly dependent on the platform and on how the app was
           called. Sometimes the first entry can be a full path. Sometimes it is
           a relative path or just a file name. On some platforms it can also
           simply be an empty string.

            The remaining items are the parameters passed to the app by the
           caller.*/
        const Array<String> &getArguments() const { return _arguments; }

        /** Sets the commandline arguments - see getArguments().*/
        void setArguments(const Array<String> &args) { _arguments = args; }

      protected:
        Array<String> _arguments;
    };
}
