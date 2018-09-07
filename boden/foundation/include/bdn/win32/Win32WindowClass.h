#ifndef BDN_WIN32_Win32WindowClass_H_
#define BDN_WIN32_Win32WindowClass_H_

#include <windows.h>

namespace bdn
{
    namespace win32
    {

        /** Base class for Win32 window class objects.
            See Win32Window.
        */
        class Win32WindowClass : public Base
        {
          public:
            Win32WindowClass(const String &name, WNDPROC windowProc);

            /** Registers the class if it is not yet registered.

                This is usually called by derived classes at the end of their
               constructor.
            */
            void ensureRegistered();

            String getName() const { return _name; }

          protected:
            String _name;
            WNDCLASSEX _info;
            bool _registered;
        };
    }
}

#endif