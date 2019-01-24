#pragma once

#include <bdn/View.h>

#include <bdn/IWindowCore.h>
#include <bdn/UiProvider.h>

namespace bdn
{

    /** A top level window.

        Windows have a single child view (the "content view") that displays the
       window contents. Usually this content view will be a view container
       object, which can then contain multiple child views.

        Windows are initially invisible. Once you have finished initializing
       your window you need to explicitly show it by setting the property
       visible=true.

        Example:

        \code

        std::shared_ptr<Window> window = std::make_shared<Window>();

        window->setTitle( "My Window Title" );

        window->setVisible( true );

        // the window is now visible on the screen.

        \endcode
    */
    class Window : public View
    {
      public:
        /** @param uiProvider the UI provider that the window should use.
                See the UiProvider documentation for more information.
                If this is nullptr then the UI provider provided by the
                app controller is used (see AppControllerBase::getUiProvider() )
           .*/
        Window(std::shared_ptr<UiProvider> uiProvider = nullptr);

      public:
        /** The window's title property.

            Depending on the platform, the title may or may not be visible in a
           window title bar on the screen. On some platforms a window does not
           have a title bar, so the title may be invisible. However, it can also
           be used in other places (e.g. when switching between windows, etc.),
            so it should always be set to a reasonable string.

            It is safe to use the property from any thread.
            */
        Property<String> title;

      public:
        /** Sets the specified view as the content view of the window.
            Note that windows can only have a single child content view. If one
           is already set then it will be replaced. See #Window class
           documentation for more information.*/
        void setContentView(std::shared_ptr<View> contentView);

        /** Returns the window's content view (see #getContentView()).
            This can be nullptr if no content view has been set yet.*/
        std::shared_ptr<View> getContentView();

        /** Returns the window's content view (see #getContentView()).
            This can be nullptr if no content view has been set yet.*/
        std::shared_ptr<const View> getContentView() const;

        /** Tells the window to auto-size itself. The window size will be
           adapted according to the preferred size of the content view. The
           window also takes other considerations into account, like the size of
           the screen it is on.

            Note that the update happens asynchronously, so the window will
           usually not have the new size yet when the function returns.
            */
        void requestAutoSize();

        /** Tells the window to center itself on the screen.

            The update will happen asynchronously, so the position might not
           have been updated yet when the function returns.

            If another asynchronous operation has been scheduled (like
           requestAutoSize()) then it is guaranteed that the centering happens
           after that operation is complete.
        */
        void requestCenter();

        /** Static function that returns the type name for #Window objects.*/
        static constexpr char coreTypeName[] = "bdn.WindowCore";

        String getCoreTypeName() const override { return coreTypeName; }

        std::list<std::shared_ptr<View>> getChildViews() const override;

        void removeAllChildViews() override;

        std::shared_ptr<View> findPreviousChildView(std::shared_ptr<View> childView) override;

        void _childViewStolen(std::shared_ptr<View> childView) override;

      protected:
        std::shared_ptr<UiProvider> determineUiProvider(std::shared_ptr<View> parentView = nullptr) override;
        virtual void _initCore() override;
        virtual void _deinitCore() override;

      private:
        std::shared_ptr<View> _contentView;
    };
}
