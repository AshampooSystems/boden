#pragma once

namespace bdn
{
    class View;
}

#include <bdn/UIMargin.h>
#include <bdn/Rect.h>
#include <bdn/Size.h>
#include <bdn/round.h>
#include <optional>

namespace bdn
{

    class IViewCore
    {
      public:
        enum class InvalidateReason
        {
            /** The data was invalidated because a standard property of the view
             (i.e. a property that is defined by the Boden framework) has been
             changed.*/
            standardPropertyChanged,

            /** The data was invalidated because a standard child property
             (i.e. a property that is defined by the Boden framework) has been
             changed.*/
            standardChildPropertyChanged,

            /** A child view was added or removed.*/
            childAddedOrRemoved,

            /** The sizing information of a child view has been invalidated. The
             child size must be recalculated.*/
            childSizingInfoInvalidated,

            /** The data was invalidated because some custom data associated
             with the view has changed that influences sizing or layout.

             This is usually used when the application overloaded layout or
             sizing functionality of the view. When some internal custom data
             changes that influences the layout then the application should use
             this update reason.
             */
            customDataChanged,
        };

        enum class HorizontalAlignment
        {
            left,
            center,
            right,
            /** Expand the view to the full available width.*/
            expand
        };

        enum class VerticalAlignment
        {
            top,
            middle,
            bottom,
            /** Expand the view to the full available height.*/
            expand
        };

        /** Invalidates the any cached sizing information of the view (see
           calcPreferredSize()).

            The outer View object automatically invalidates its own sizing info
           cache and also notifies the parent view, that a child's sizing info
           has been invalidated.

            So this IViewCore version of the function only has to invalidate any
           cached sizing information that it keeps itself.

            \param reason the reason for the update. If the function is called
           by the application (rather than the framework itself) then this
           should usually be set to View::InvalidateReason::customDataChanged
            */
        virtual void invalidateSizingInfo(InvalidateReason reason) = 0;

        /** Requests that the view updates the layout of its child view and
           contents.

            The layout operation does not happen immediately in this function -
           it is performed asynchronously.

            View core implementations may use their platform's native layout
           system to schedule the update. They can also use the generic
           bdn::LayoutCoordinator class or a custom implementation.

            The view should NOT update its OWN size or position during the
           layout operation - it has to work with the size and position it
           currently has and should ONLY update the size and position of its
           child views.

            Implementation tips for specific view types
            -------------------------------------------------

            - ContainerView cores should call ContainerView::calcContainerLayout
              and then ViewLayout::applyTo to calculate and then apply the
           layout.

            - Window cores can use the global function defaultWindowLayoutImpl()
              for a default implementation, if they do not need special
           handling.

            - Simple controls without children often do nothing in layout and
           can ignore the needLayout() call.


            Note to layout implementors
            --------------------

            Depending on the UI implementation backend,
            the sizes and positions of child views may have some constraints.
           For example, with many implementations the sizes and positions must
           be rounded to full physical pixel boundaries. The layout function
           should be aware of this and use adjustBounds() to calculate a bounds
           rect that meets these constraints.

            When calling adjustBounds in this context, it is recommended to use
           RoundType::up for rounding child view positions. That ensures that
           small margins that are less than 1 pixel in size are rounded up to 1
           pixel, rather than disappearing completely.

            Child view sizes should usually be rounded with RoundType::up when
           enough space is available and RoundType::down when not enough space
           is available.

            The rounding policies noted above are merely guidelines: layout
           implementations are free to ignore them if there are other
           considerations that cause other rounding types to be better for for
           the particular case.

            \param reason the reason for the update. If the function is called
           by the application (rather than the framework itself) then this
           should usually be set to View::UpdateReason::customDataChanged
            */
        virtual void needLayout(InvalidateReason reason) = 0;

        /** This is called when the sizing information of a child view was
           invalidated.

            The normal implementation for this is that this invalidates the
           view's own sizing info and layout. But the core has full control over
           this and can use an optimized implementation if it wants to.
            */
        virtual void childSizingInfoInvalidated(std::shared_ptr<View> child) = 0;

        /** Sets the view core's horizontal alignment. See
         * View::horizontalAlignment() */
        virtual void setHorizontalAlignment(const HorizontalAlignment &align) = 0;

        /** Sets the view core's vertical alignment. See
         * View::verticalAlignment() */
        virtual void setVerticalAlignment(const VerticalAlignment &align) = 0;

        /** Shows/hides the view core.*/
        virtual void setVisible(const bool &visible) = 0;

        /** Sets the view core's padding. See View::padding() */
        virtual void setPadding(const std::optional<UIMargin> &padding) = 0;

        /** Sets the view core's margin. See View::margin() */
        virtual void setMargin(const UIMargin &margin) = 0;

        /** Sets the view core's preferred size hint.*/
        virtual void setPreferredSizeHint(const Size &hint) = 0;

        /** Sets the view core's preferred size minimum.*/
        virtual void setPreferredSizeMinimum(const Size &limit) = 0;

        /** Sets the view core's preferred size maximum.*/
        virtual void setPreferredSizeMaximum(const Size &limit) = 0;

        /** Sets the view's position and size, after adjusting the specified
           values to ones that are compatible with the underlying view
           implementation. The bounds are specified in DIP units and refer to
           the parent view's coordinate system.

            See adjustBounds() for more information about the adjustments that
           are made.

            Note that the adjustments are made with a "nearest valid" policy.
           I.e. the position and size are set to the closest valid value. This
           can mean that the view ends up being bigger or smaller than
           requested. If you need more control over which way the adjustments
           are made then you should pre-adjust the bounds with adjustBounds().

            The function returns the adjusted bounds that are actually used.
            */
        virtual Rect adjustAndSetBounds(const Rect &requestedBounds) = 0;

        /** Adjusts the specified bounds to values that are compatible with the
           underlying view implementation and returns the result. The bounds are
           specified in DIP units and refer to the parent view's coordinate
           system.

            IMPORTANT: This function must only be called from the main thread.

            Not all positions and sizes are necessarily valid for all view
           implementations. For example, the backend might need to round the
           abstract DIP coordinates to the nearest physical pixel boundary.

            The function adjusts the specified bounds according to its
           implementation constraints and returns the valid values. The
           positionRoundType and sizeRoundType control in which direction
           adjustments are made (adjusting up, down or to the nearest valid
           value).
        */
        virtual Rect adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                                  RoundType sizeRoundType) const = 0;

        /** Converts the specified UI length to DIPs (see \ref dip.md).

            Return 0 for "none" values (see UILength::isNone()).

        */
        virtual double uiLengthToDips(const UILength &uiLength) const = 0;

        /** Converts a UIMargin object to a DIP-based margin object (see \ref
         * dip.md).*/
        virtual Margin uiMarginToDipMargin(const UIMargin &margin) const = 0;

        /** Asks the view to calculate its preferred size in DIPs (see \ref
           dip.md), based on it current contents	and properties.

            There are several constraints for the preferred size:

            availableSpace
            --------------

            The availableSpace function parameter is used to indicate the
           maximum amount of available space for the view (also in DIPs). If
           availableSpace is Size::none() (i.e. width and height equal
           Size::componentNone()) then that means that the available space
           should be considered to be unlimited. I.e. the function should return
           the view's optimal size.

            When one of the availableSpace components (width or height) is not
           Size::componentNone() then it means that the available space is
           limited in that dimension. The function should return the preferred
           size of the view within those constraints, trying to not exceed the
           limited size component.

            For example, many views displaying text can handle a limited
           available width by wrapping the text into multiple lines (and thus
           increasing their height).

            If the view cannot reduce its size to fit into the available space
           then it is valid for the function to return a size that exceeds the
           available space. However, the layout manager is free to size the view
           to something smaller than the returned preferred size.

            preferredSizeHint
            -----------------

            preferredSizeHint is an optional advisory hint to the view as to
           what the preferred width and/or height should roughly be. The
           calcPreferredSize implementation may ignore this if it does not make
           sense for the view type. In fact the value is unused by most views.
           One example where the parameter can be useful are text views which
           can dynamically wrap text into multiple lines. These kinds of views
           can use the hint width to determine the place where the text should
            wrap by default

            preferredSizeMinimum and preferredSizeMaximum
            ---------------------------------------------

            preferredSizeMinimum and preferredSizeMaximum are hard limits for
           the preferred size. The calcPreferredSize implementation should never
           return a size that violates these limits, if they are set. Even if
           that means that the view's content does not fit into the view.

            If there is a conflict between the minimum and maximum and/or hint
           values then the values should be prioritized in this ascending order:
           hint, minimum, maximum. So the maximum value has the highest priority
           and the returned value should never exceed it. For example, if a
           minimum is set that exceeds the maximum then the maximum should "win"
           and the preferred size should not exceed the maximum.


            Implementation tips for specific view types
            -------------------------------------------------

            - Window cores can use the global function
           defaultWindowCalcPreferredSizeImpl() for a default implementation, if
           they do not need special handling.


            Important Notes
            ---------------

            IMPORTANT: It is perfectly ok (even recommended) for the view to
           return a preferred size that is not adjusted for the properties of
           the current display / monitor yet. I.e. it may not be rounded to full
           physical pixels yet. The size will be adapted to the display
           properties in adjustAndSetBounds().

            IMPORTANT: This function must only called be called from the main
           thread.
            */
        virtual Size calcPreferredSize(const Size &availableSpace = Size::none()) const = 0;

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

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const IViewCore::HorizontalAlignment a)
    {
        switch (a) {
        case IViewCore::HorizontalAlignment::left:
            return stream << "left";
        case IViewCore::HorizontalAlignment::center:
            return stream << "center";
        case IViewCore::HorizontalAlignment::right:
            return stream << "right";
        case IViewCore::HorizontalAlignment::expand:
            return stream << "expand";
        }
        return stream << "fail";
    }

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const IViewCore::VerticalAlignment a)
    {
        switch (a) {
        case IViewCore::VerticalAlignment::top:
            return stream << "top";
        case IViewCore::VerticalAlignment::middle:
            return stream << "middle";
        case IViewCore::VerticalAlignment::bottom:
            return stream << "bottom";
        case IViewCore::VerticalAlignment::expand:
            return stream << "expand";
        }

        return stream << "fail";
    }
}

#include <bdn/View.h>
