#pragma once

#include <bdn/View.h>

#include <bdn/ViewCoreFactory.h>
#include <bdn/WindowCore.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Window)
    }

    class Window : public View
    {
      public:
        using Orientation = WindowCore::Orientation;

      public:
        Window(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        Property<Rect> contentGeometry;
        Property<String> title;
        Property<std::shared_ptr<View>> content;

        Property<Orientation> allowedOrientations;
        Property<Orientation> currentOrientation;

      public:
        static constexpr char coreTypeName[] = "bdn.WindowCore";
        String viewCoreTypeName() const override { return coreTypeName; }

      public:
        std::list<std::shared_ptr<View>> childViews() override;
        void removeAllChildViews() override;

        void childViewStolen(const std::shared_ptr<View> &childView) override;

      protected:
        void bindViewCore() override;

      private:
        SingleChildHelper _content;
    };
}
