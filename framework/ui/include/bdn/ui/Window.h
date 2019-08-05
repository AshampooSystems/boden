#pragma once

#include <bdn/ui/View.h>
#include <bdn/ui/ViewCoreFactory.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Window)
    }

    class Window : public View
    {
      public:
      public:
        enum Orientation : int
        {
            Portrait = 0x1,
            LandscapeLeft = 0x2,
            LandscapeRight = 0x4,
            PortraitUpsideDown = 0x8,
            LandscapeMask = (LandscapeLeft | LandscapeRight),
            PortraitMask = (Portrait | PortraitUpsideDown),
            All = (LandscapeMask | PortraitMask)
        };

        static std::string orientationToString(Orientation orientation);

      public:
        Window(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      private:
        Property<Rect> internalContentGeometry;

      public:
        const Property<Rect> &contentGeometry = internalContentGeometry;
        Property<std::string> title;
        Property<std::shared_ptr<View>> contentView;

        Property<Orientation> allowedOrientations;
        Property<Orientation> currentOrientation;

      public:
        std::list<std::shared_ptr<View>> childViews() override;
        void removeAllChildViews() override;

        void childViewStolen(const std::shared_ptr<View> &childView) override;

      protected:
        void bindViewCore() override;

      private:
        SingleChildHelper _contentView;

      public:
        class Core
        {
          public:
            using Orientation = Window::Orientation;

          public:
            Property<std::shared_ptr<View>> contentView;
            Property<Rect> contentGeometry;
            Property<std::string> title;

            Property<Orientation> allowedOrientations;
            Property<Orientation> currentOrientation;
        };
    };
}
