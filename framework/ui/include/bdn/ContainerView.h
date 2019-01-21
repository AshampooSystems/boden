#pragma once

#include <bdn/View.h>
#include <bdn/ViewLayout.h>

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
        static String getContainerViewCoreTypeName() { return "bdn.ContainerViewCore"; }

        String getCoreTypeName() const override { return getContainerViewCoreTypeName(); }

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

        std::list<std::shared_ptr<View>> getChildViews() const override;

        std::shared_ptr<View> findPreviousChildView(std::shared_ptr<View> childView) override;

        void _childViewStolen(std::shared_ptr<View> childView) override;

        /** Calculates the layout for the container based on the specified total
           container size. The sizes and positions of the child views are
           calculated and stored in the returned layout object. The layout can
           then be applied later by calling ViewLayout::applyTo().

            calcContainerLayout may be called multiple times to create multiple
           layout objects for different sizes. Any of the created layouts may be
           applied later with ViewLayout::applyTo(). It is also valid to apply
           none of them and throw them away.

            \param containerSize the size of the container view to use as the
           basis for the layout. This does not have to match the current size of
           the container view.
            */
        virtual std::shared_ptr<ViewLayout> calcContainerLayout(const Size &containerSize) const = 0;

        /** Calculates the preferred size for the container. Container
           implementations must override this to implement their custom size
           calculation.

            calcContainerPreferredSize is automatically called from
           View::calcPreferredSize() and has the same behaviour.

            */
        virtual Size calcContainerPreferredSize(const Size &availableSpace = Size::none()) const = 0;

      protected:
        std::list<std::shared_ptr<View>> _childViews;
    };
}
