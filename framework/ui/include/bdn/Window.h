#pragma once

#include <bdn/View.h>

#include <bdn/UIProvider.h>
#include <bdn/WindowCore.h>

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
                See the UIProvider documentation for more information.
                If this is nullptr then the UI provider provided by the
                app controller is used (see AppControllerBase::uiProvider() )
           .*/
        Window(std::shared_ptr<UIProvider> uiProvider = nullptr);

      public:
        Property<Rect> contentGeometry;
        Property<String> title;
        Property<std::shared_ptr<View>> content;

      public:
        /** Static function that returns the type name for #Window objects.*/
        static constexpr char coreTypeName[] = "bdn.WindowCore";

        String viewCoreTypeName() const override { return coreTypeName; }

        std::list<std::shared_ptr<View>> childViews() override;

        void removeAllChildViews() override;

        void childViewStolen(const std::shared_ptr<View> &childView) override;

      protected:
        void bindViewCore() override;

      private:
        SingleChildHelper _content;
    };
}
