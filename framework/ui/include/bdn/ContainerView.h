#pragma once

#include <bdn/View.h>

#include <list>

namespace bdn
{

    /** Base class for views that contains multiple child views.

        Derived classes must override the calcContainerLayout() and
       calcContainerPreferredSize() methods.

        */
    class ContainerView : public View
    {
      public:
        ContainerView() = default;

        /** Static function that returns the type name for #ContainerView core
         * objects.*/

        static constexpr char coreTypeName[] = "bdn.ContainerViewCore";

        String viewCoreTypeName() const override { return coreTypeName; }

        /** Adds a child to the end of the container.

            If the child view is already a child of this container then it
            is moved to the end.
        */
        void addChildView(std::shared_ptr<View> childView);

        /** Inserts a child before another child.

            If insertBeforeChildView is nullptr then the new view is added to
           the end of the container.

            If insertBeforeChildView is not a child of this container then the
           new child view is added to the end of the container.

            If the child view is already a child of this container then it
            is moved to the desired target position.
            */
        void insertChildView(std::shared_ptr<View> insertBeforeChildView, std::shared_ptr<View> childView);

        /** Removes the specified child view from the container.

            Has no effect if the specified view is not currently a child of this
           container.
        */
        void removeChildView(std::shared_ptr<View> childView);

        void removeAllChildViews() override;

        std::list<std::shared_ptr<View>> childViews() const override;

        void _childViewStolen(std::shared_ptr<View> childView) override;

      protected:
        std::list<std::shared_ptr<View>> _childViews;
    };
}
