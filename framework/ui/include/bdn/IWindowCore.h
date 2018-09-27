#ifndef BDN_IWindowCore_H_
#define BDN_IWindowCore_H_

#include <bdn/IViewCore.h>

namespace bdn
{

    /** The core for a top level window.*/
    class IWindowCore : BDN_IMPLEMENTS IViewCore
    {
      public:
        /** Tells the window to auto-size itself. The window size will be
           adapted according to the preferred size of the content view. The
           window also takes other considerations into account, like the size of
           the screen it is on.

            Note that the update happens asynchronously, so the window will
           usually not have the new size yet when the function returns.

            Usually the auto-sizing is done by the same layout system that also
            processes IViewCore::needLayout() requests.

            The auto sizing should happen at a point in time when all pending
           sizing info updates have already been processed.

            defaultWindowAutoSizeImpl() provides a default implementation for
           auto sizing the window. IWindowCore implementations can use this to
           implement the actual centering.
            */
        virtual void requestAutoSize() = 0;

        /** Tells the window to center itself on the screen.

            The update will happen asynchronously, so the position might not
           have been updated yet when the function returns.


            Usually the centering is done by the same layout system that also
            processes IViewCore::needLayout() requests.

            The centering should happen at a point in time when all pending
           sizing info updates and auto sizing requests have already been
           processed.

            defaultWindowCenterImpl() provides a default implementation for
           centering a window. IWindowCore implementations can use this to
           implement the centering.
        */
        virtual void requestCenter() = 0;

        /** Changes the window's title.*/
        virtual void setTitle(const String &title) = 0;
    };
}

#endif
