#pragma once

#include <bdn/Rect.h>
#include <bdn/Size.h>
#include <bdn/property/Property.h>
#include <bdn/round.h>
#include <optional>

namespace bdn
{
    class View;

    class ViewCore
    {
      public:
        Property<Rect> geometry;
        Property<bool> visible;

      public:
        virtual void scheduleLayout() = 0;

        virtual Size sizeForSpace(Size availableSize = Size::none()) const { return Size{0, 0}; }

        /** Called by the framework when the parent view is being changed to
           check whether it is possible to move the core to a new parent without
           reinitializing it.

            Returns true if it is possible to move the core to a new parent or
           false otherwise.
            */
        virtual bool canMoveToParentView(std::shared_ptr<View> newParentView) const = 0;

        /** Called when the outer view's parent is being changed and the core
           needs to move to a new parent.

            This function moves the core over to the given new parent view
           without reinitializing it.

            On some platforms, it may not be possible to move the core to a new
           parent view directly. The framework calls canMoveToParentView()
           before calling this method and it will only call moveToParentView()
           if a prior call to canMoveToParentView() returned true.
            */
        virtual void moveToParentView(std::shared_ptr<View> newParentView) = 0;

        /** Called when the outer view's parent has changed and the framework is
           not able to move the view's core to the new parent, or if the parent
           is being set to nullptr and the core needs to be disposed.
            */
        virtual void dispose() = 0;
    };
}
