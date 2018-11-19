#ifndef BDN_LayoutCoordinator_H_
#define BDN_LayoutCoordinator_H_

#include <bdn/Window.h>
#include <bdn/View.h>

#include <bdn/Set.h>

namespace bdn
{

    /** Coordinates the updating of the layout for user interface components.

        The coordinator can be used by IViewCore implementations to implement
       coordination of layout functions.

        Whenever an event happens that might require a re-layout, the
       corresponding component notifies the layout coordinator.

        The coordinator collects these requests and tries to batch together
        multiple consecutive changes that modify the layout into a single update
       operation. Note that the layout update will still happen almost
       immediately.

        The coordinator also optimizes the order of multiple update operations,
        to ensure that no duplicate work is done.

        Usually the global coordinator object should be used (see
       LayoutCoordinator::get()).

        The view core objects that use the layout coordinator need to implement
       IViewCoreExtension. or IWindowCoreExtension (depending on the the type of
       core object).
    */
    class LayoutCoordinator : public Base
    {
      public:
        LayoutCoordinator();

        /** Registers a view for re-layout. This should be called when any of
           the child views have changed their size or any of the parameters that
           affect their layout (like margins, alignment, etc.)
            */
        void viewNeedsLayout(View *view);

        /** Registers a top level window for auto-sizing.*/
        void windowNeedsAutoSizing(Window *window);

        /** Registers a top-level window for centering on the screen.*/
        void windowNeedsCentering(Window *window);

        /** Interface that IViewCore objects which use the LayoutCoordinator
         * should implement.*/
        class IViewCoreExtension : BDN_IMPLEMENTS IBase
        {
          public:
            /** Updates the layout of the view's contents (see
             * View::needLayout()).*/
            virtual void layout() = 0;
        };

        /** Interfaces that IWindowCore objects which use the LayoutCoordinator
         * should implement.*/
        class IWindowCoreExtension : BDN_IMPLEMENTS IViewCoreExtension
        {
          public:
            /** Autosizes the window. See Window::requestAutoSize().*/
            virtual void autoSize() = 0;

            /** Centers the window on the screen. See Window::requestCenter()*/
            virtual void center() = 0;
        };

      protected:
        void needUpdate();

        void updateNow();

        /** This function is called when an exception occurs during while a
            view is being updated. The default implementation logs the exception
            and does nothing otherwise.

            \param exceptionIfAvailable is non-null if the exception is a
           std::exception object. It is null if it is another kind of exception.

            \param functionName is the name of the function that produced the
           exception.

            */
        virtual void handleException(const std::exception *exceptionIfAvailable, const String &functionName);

        Set<P<View>> _layoutSet;

        Set<P<Window>> _windowAutoSizeSet;
        Set<P<Window>> _windowCenterSet;

        bool _updateScheduled = false;

        bool _inUpdateNow = false;
    };
}

#endif
