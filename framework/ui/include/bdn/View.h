#ifndef BDN_View_H_
#define BDN_View_H_

namespace bdn
{
    class View;
    class IViewCore;   // included below
    class IUiProvider; // included below
}

#include <bdn/UiMargin.h>
#include <bdn/UiSize.h>
#include <bdn/Rect.h>
#include <bdn/Nullable.h>
#include <bdn/RequireNewAlloc.h>
#include <bdn/property.h>
#include <bdn/mainThread.h>
#include <bdn/round.h>
#include <bdn/PreferredViewSizeManager.h>
#include <bdn/List.h>

#include <bdn/IViewCore.h>

/** \def BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS( ValueType, readAccess, name,
   writeAccess, setterName, CoreInterfaceType, modificationInfluenceCalls )

    Like \ref BDN_VIEW_PROPERTY, except that one can specify the access
   permissions for read and write access (public, protected, private). See \ref
   BDN_VIEW_PROPERTY for more information.

    \param ValueType The type of the internal property value. This must be a
   valid C++ type or class name. \param readAccess Access specifier for read
   access. This is one of the normal C++ access specifiers: public, protected,
   private. It defines the access for the getter function, the function that
   returns the changed notifier and the PropertyValueType_NAME typedef. \param
   name The name of the property \param writeAccess Access specifier for write
   access. This is one of the normal C++ access specifiers: public, protected,
   private. It defines the access for the setter function. \param setterName The
   name of the property's setter function \param CoreInterfaceType The type of
   the view's core interface (e.g. IButtonCore for buttons) \param
   modificationInfluenceCalls Calls that represent the influences of the
   property on the layout system. See the example in the documentation of \ref
   BDN_VIEW_PROPERTY.
    */
#define BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS(                                  \
    ValueType, readAccess, name, writeAccess, setterName, CoreInterfaceType,   \
    modificationInfluenceCalls)                                                \
    readAccess:                                                                \
    virtual ValueType name() const { return _propertyValue_##name; }           \
    writeAccess:                                                               \
    virtual void setterName(const ValueType &newValue)                         \
    {                                                                          \
        if (_propertyValue_##name != newValue) {                               \
            _propertyValue_##name = newValue;                                  \
            bdn::P<CoreInterfaceType> pCore =                                  \
                bdn::cast<CoreInterfaceType>(getViewCore());                   \
            if (pCore != nullptr)                                              \
                pCore->setterName(newValue);                                   \
            Influences_(this).modificationInfluenceCalls;                      \
            BDN_NOTIFY_PROPERTY_CHANGED(*this, name);                          \
        }                                                                      \
    }                                                                          \
                                                                               \
  private:                                                                     \
    ValueType _propertyValue_##name{};                                         \
    readAccess:                                                                \
    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION(ValueType, name);              \
    BDN_FINALIZE_CUSTOM_PROPERTY(ValueType, name, setterName);

/** \def BDN_VIEW_PROPERTY( ValueType, name, setterName, CoreInterfaceType,
   modificationInfluenceCalls )

    Defines a property of a view class.

    These are extended versions of "normal" properties (see \ref BDN_PROPERTY).
    In addition to the normal property functionality, view properties also do
   the following:

    - View properties call a method of the view core whenever the property value
   changes. The view property casts the view's core object to the interface type
      specified in the CoreInterfaceType parameter and then calls a method of
   the core with the same name as the property's setter function. For example,
   if the view property is `label()` with a setter `setLabel()` and the
      CoreInterfaceType is `IButtonCore` then `IButtonCore::setLabel(newLabel)`
   is called on the view core.

    - View properties update the layout system as needed whenever the property
   value changes. The modificationInfluenceCalls parameter specifies which
   aspects of the layout are invalidated by the property modification. Possible
   influence calls are:

      - influencesNothing(): the property is irrelevant for the layout
      - influencesContentLayout(): the property value influences the layout of
   the view's content
      - influencesParentPreferredSize(): the property value influences the
   preferred size (and consequently also the layout) of its parent view.
      - influencesPreferredSize(): the property value influences the view's
   preferred size. This automatically implies influencesParentPreferredSize()
   and influencesContentLayout()

      Multiple influence calls can be combined by placing a dot in between. For
   example: `influencesContentLayout() . influencesParentPreferredSize()` states
   that both the content layout and the parent's preferred size are influenced
   by the property.

    Example:

    \code

        // A fictional custom view that displays an image
        class MyImageView : public View
        {
        public:

            // the following creates a property that calls
   IMyImageViewCore::setImage on the core
            // whenever the value changes. Additionally, the view's preferred
   size and its parent's
            // preferred size are invalidated whenever the value changes.
            BDN_VIEW_PROPERTY( Image, image, setImage, IMyImageViewCore,
   influencesPreferredSize() . influencesParentPreferredSize() ):
        };

    \endcode

    Note that there is also a variant of this macro that does not make the view
   core call (see \ref BDN_VIEW_PROPERTY_WITHOUT_CORE_FORWARDING). There is also
   a variant that enables specifying custom access specifiers for read and write
   access (see \ref BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS).

    \param ValueType The type of the internal property value. This must be a
   valid C++ type or class name. \param name The name of the property \param
   setterName The name of the property's setter function \param
   CoreInterfaceType The type of the view's core interface (e.g. IButtonCore for
   buttons) \param modificationInfluenceCalls Calls that represent the
   influences of the property on the layout system. See above for more
   information.
    */
#define BDN_VIEW_PROPERTY(ValueType, name, setterName, CoreInterfaceType,      \
                          modificationInfluenceCalls)                          \
    BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS(ValueType, public, name, public,      \
                                         setterName, CoreInterfaceType,        \
                                         modificationInfluenceCalls);

/** Creates a view property that works like those created by \ref
   BDN_VIEW_PROPERTY, except that it does **not** call any method on the view
   core when the property value changes. Apart from that, it behaves exactly the
   same.

    See \ref BDN_VIEW_PROPERTY for details.

    \param ValueType The type of the internal property value. This must be a
   valid C++ type or class name. \param name The name of the property \param
   setterName The name of the property's setter function \param
   modificationInfluenceCalls Calls that represent the influences of the
   property on the layout system. See \ref BDN_VIEW_PROPERTY for more
   information.
 */
#define BDN_VIEW_PROPERTY_WITHOUT_CORE_FORWARDING(ValueType, name, setterName, \
                                                  modificationInfluenceCalls)  \
    BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS_WITHOUT_CORE_FORWARDING(              \
        ValueType, public, name, public, setterName,                           \
        modificationInfluenceCalls);

/** Creates a view property that has custom access rights for read and write
   access and that also does not forward modifications of the property value to
   the view's core.

    The created property works like those created by \ref
   BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS, except that it does **not** call any
   method on the view core when the property value changes. Apart from that, it
   behaves exactly the same.

    Also see \ref BDN_VIEW_PROPERTY for general information on view properties.

    \param ValueType The type of the internal property value. This must be a
   valid C++ type or class name. \param readAccess Access specifier for read
   access. This is one of the normal C++ access specifiers: public, protected,
   private. It defines the access for the getter function, the function that
   returns the changed notifier and the PropertyValueType_NAME typedef. \param
   name The name of the property \param writeAccess Access specifier for write
   access. This is one of the normal C++ access specifiers: public, protected,
   private. It defines the access for the setter function. \param setterName The
   name of the property's setter function \param modificationInfluenceCalls
   Calls that represent the influences of the property on the layout system. See
   the example in the documentation of \ref BDN_VIEW_PROPERTY.
 */
#define BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS_WITHOUT_CORE_FORWARDING(          \
    ValueType, readAccess, name, writeAccess, setterName,                      \
    modificationInfluenceCalls)                                                \
    readAccess:                                                                \
    virtual ValueType name() const { return _propertyValue_##name; }           \
    writeAccess:                                                               \
    virtual void setterName(const ValueType &newValue)                         \
    {                                                                          \
        if (_propertyValue_##name != newValue) {                               \
            _propertyValue_##name = newValue;                                  \
            Influences_(this).modificationInfluenceCalls;                      \
            BDN_NOTIFY_PROPERTY_CHANGED(*this, name);                          \
        }                                                                      \
    }                                                                          \
                                                                               \
  private:                                                                     \
    ValueType _propertyValue_##name{};                                         \
    readAccess:                                                                \
    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION(ValueType, name);              \
    BDN_FINALIZE_CUSTOM_PROPERTY(ValueType, name, setterName);

namespace bdn
{

    /** Views are the building blocks of the visible user interface.
        A view presents data or provides some user interface functionality.
        For example, buttons, text fields etc are all view objects.

        View objects must be allocated with newObj or new.
       */
    class View : public RequireNewAlloc<Base, View>
    {
      public:
        View();
        ~View();

        // delete copy constructor
        View(const View &o) = delete;

        /** Returns the core object of this view.

            The core can be null if the view is not currently connected
           (directly or indirectly) to a top level window. It can also be null
           for short periods of time when a reinitialization was necessary.

            The core provides the actual implementation of the view. It is
           provided by the IUiProvider object that the view uses. The
           IUiProvider is inherited from the parent view and can be explicitly
           set when creating a top level window.
            */
        P<IViewCore> getViewCore() const { return _pCore; }

        /** Controls wether the view is visible or not.

            Note that a view with visible=true might still not show on
            the screen if one of its parents is invisible. In other words:
            this visible property only raefers to the view itself, not
            the parent hierarchy.
            */
        BDN_VIEW_PROPERTY(bool, visible, setVisible, IViewCore,
                          influencesNothing());

        /** The size of the empty space that should be left around the view.

            The margin is NOT part of the view itself. It is merely something
           that the layout takes into account.

            The default margin is 0.
        */
        BDN_VIEW_PROPERTY(
            UiMargin, margin, setMargin, IViewCore,
            influencesParentPreferredSize().influencesParentLayout());

        /** The size space around the content inside this view.

            The padding is part of the view and thus it influences the size of
            the view (in contrast to the margin(), which is NOT part of the
           view).

            On some platforms some UI elements may have a built-in minimum for
           the padding. If you specify a smaller padding then the minimum
           padding will be silently used instead.

            The padding can also be "null", which means that the implementation
            should use the "normal" or "most commonly used" padding for the UI
           element on the platform.

            The default padding is "null".
        */
        BDN_VIEW_PROPERTY(Nullable<UiMargin>, padding, setPadding, IViewCore,
                          influencesPreferredSize().influencesContentLayout());

        /** The position of the view, in client coordinates of the parent view.
            ) in DIP units (see \ref dip.md).

            The position property is read-only. The position of a view can be
           modified with adjustAndSetBounds, which is usually called
           automatically during the parent view's layout process.

            The default position for a newly constructed view is always position
           0,0. The position is usually initialized automatically by the parent
           view's layout routine.

            IMPORTANT:

            On some platforms top level windows (see #Window class) may report a
           zero position at all times, even though the window is not at the top
           left corner of the screen.
        */
        BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS_WITHOUT_CORE_FORWARDING(
            Point, public, position, protected, _setPosition,
            influencesNothing());

        /** The size of the view DIP units (see \ref dip.md).

            The size property is read-only. The size of a view can be modified
           with adjustAndSetBounds, which is usually called automatically during
           the parent view's layout process.

            The default size for a newly constructed view is always 0x0.
            The size is usually set automatically by the parent view's layout
           routine.
        */
        BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS_WITHOUT_CORE_FORWARDING(
            Size, public, size, protected, _setSize, influencesContentLayout());

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

            IMPORTANT:

            The position and/or size of top level #Window objects are restricted
           on some platforms. Sometimes it is not possible to change the top
           level Window bounds at all (in that case the bounds will be
            "adjusted" to the current bounds value).

            On some platforms top level windows may also report a zero position
           at all times, even though the window is not at the top left corner of
           the screen.
            */
        virtual Rect adjustAndSetBounds(const Rect &requestedBounds);

        /** Adjusts the specified bounds to values that are compatible with the
           underlying view implementation and returns the result. The bounds are
           specified in DIP units and refer to the parent view's coordinate
           system.

            Not all positions and sizes are necessarily valid for all view
           implementations. For example, the backend might need to round the
           abstract DIP coordinates to the nearest physical pixel boundary.

            The function adjusts the specified bounds according to its
           implementation constraints and returns the valid values. The
           positionRoundType and sizeRoundType control in which direction
           adjustments are made (adjusting up, down or to the nearest valid
           value).

            IMPORTANT:

            The position and/or size of top level #Window objects are restricted
           on some platforms. Sometimes it is not possible to change the top
           level Window bounds at all (in that case the bounds will be
            "adjusted" to the current bounds value).

            On some platforms top level windows may also report a zero position
           at all times, even though the window is not at the top left corner of
           the screen.
        */
        // XXX: "adjustBounds" could be understood as modifying the bounds of
        // the view that it's called on. Suggestion: rename to "adjustedBounds"
        // – that's also what the variable is called in
        // ColumnView::calcLayoutImpl().
        virtual Rect adjustBounds(const Rect &requestedBounds,
                                  RoundType positionRoundType,
                                  RoundType sizeRoundType) const;

        using HorizontalAlignment = IViewCore::HorizontalAlignment;
        using VerticalAlignment = IViewCore::VerticalAlignment;

        /** Controls how the view is arranged vertically if
            there is additional vertical free space. Parent view containers can
           ignore this setting if it does not make sense in their context.

            The default alignment is View::VerticalAlignment::top.
            */
        BDN_VIEW_PROPERTY(VerticalAlignment, verticalAlignment,
                          setVerticalAlignment, IViewCore,
                          influencesParentLayout());

        /** Controls how the view is arranged horizontally if
            there is additional horizontal free space. Parent view containers
           can ignore this setting if it does not make sense in their context.

            The default alignment is View::HorizontalAlignment::left.
            */
        BDN_VIEW_PROPERTY(HorizontalAlignment, horizontalAlignment,
                          setHorizontalAlignment, IViewCore,
                          influencesParentLayout());

        /*

        virtual Property<double>& extraSpaceWeight()
        {
            return _extraSpaceWeight;
        }

        virtual const ReadProperty<double>& extraSpaceWeight() const
        {
            return _extraSpaceWeight;
        }

        */

        /** Returns the UI provider used by this view. This can be nullptr if no
           UI provider is currently associated with the view object. This can
           happen, for example, when the view object is not yet connected to a
           top level window (either directly or indirectly).

            Note that there can sometimes be a short delay after a window has
           been added to a new parent until its UI provider becomes available in
           the child view.
            */
        P<IUiProvider> getUiProvider() { return _pUiProvider; }

        /** Returns the type name of the view core. This is a somewhat arbitrary
           name that is used in the internal implementation. It is NOT
           necessarily the same as the name of the C++ class of the view or view
           core (although it is often similar).
            */
        virtual String getCoreTypeName() const = 0;

        /** Returns the view's parent view. This can be null under any of the
           following circumstances:

            - the view is a top level window and does not have a parent
            - the view was not added to a parent yet
            - the parent was deleted or is about to be deleted.*/
        virtual P<View> getParentView() { return _parentViewWeak.toStrong(); }

        /** Stores a list with all the child views in the target list object.*/
        virtual void getChildViews(List<P<View>> &childViews) const
        {
            // no child views by default. So nothing to do.
        }

        /** Removes all child views.*/
        virtual void removeAllChildViews()
        {
            // no child views by default
        }

        /** Finds the child view that "precedes" the specified one.
            Returns nullptr if any of the following conditions are true:

            - the specified view is not a child of this view
            - the specified view is the first child of this view
            - this view does not define an order among its children

            */
        virtual P<View> findPreviousChildView(View *pChildView)
        {
            // no child views by default
            return nullptr;
        }

        /** Should only be called by view container implementors when they add
           or remove a child. Users of View objects should NOT call this.

            Tells the view object that it has a new parent.
            pParentView can be nullptr if the view was removed from a parent
            and does not currently have one.

            */
        void _setParentView(View *pParentView);

        /** Should only be called by view container implementations.
            Users of View objects should NOT call this.

            This must be called when another view container "steals" a view that
            was formerly a child of this view.
            */
        virtual void _childViewStolen(View *pChildView)
        {
            // do nothing by default
        }

        using InvalidateReason = IViewCore::InvalidateReason;

        /** Invalidates the cached sizing information of the view (see
           calcPreferredSize()).

            It is usually not necessary to call this manually. The view will
           automatically invalidate the sizing info when relevant internal data
           or properties change.

            Invalidating the sizing info also invalidates the layout and sizing
           info of any direct or indirect parent view(s).

            \param reason the reason for the update. If the function is called
           by the application (rather than the framework itself) then this
           should usually be set to View::InvalidateReason::customDataChanged
            */
        virtual void invalidateSizingInfo(InvalidateReason reason);

        /** Requests that the view updates the layout of its child view and
           contents.

            The layout operation does not happen immediately in this function -
           it is performed asynchronously.

            Note that it is usually NOT necessary to call this as a user of a
           view object. The view object will automatically schedule re-layout
           operations when its layout parameters or child views change.

            \param reason the reason for the update. If the function is called
           by the application (rather than the framework itself) then this
           should usually be set to View::InvalidateReason::customDataChanged
            */
        virtual void needLayout(InvalidateReason reason);

        /** An optional hint for the viewa s to how to calculate its preferred
           size. This can be set by the App to influence the automatic sizing of
           the view.

            This does *not* set a hard limit like preferredSizeMinimum or
           preferredSizeMaximum. The view is free to ignore this hint, if it
           does not make sense in the context of the view.

            But for some views the hint is used to influence the calculation of
           the preferred size. For example, text views should use the hint width
           as a guideline as to where to automatically wrap their text.

            Width and/or height of the hint can be set to Size::componentNone()
           to indicate that there is no hint for that component (i.e. the view
           should choose the preferred size completely on its own).

            The default value is Size::none(), i.e. there is no size hint.

            IMPORTANT: this property only influences the preferred size that the
           view requests during layout (see calcPreferredSize()). Its parent
           view may decide to make it bigger than this because of other layout
           considerations.
        */
        BDN_VIEW_PROPERTY(Size, preferredSizeHint, setPreferredSizeHint,
                          IViewCore, influencesPreferredSize());

        /** An optional lower limit for the preferred size of the view (in DIP
           units). This can be used by the application to influence the layout
           of the view and enforce special sizing.

            Width and/or height of the constraint can be set to
           Size::componentNone() to indicate that the corresponding component
           should not have a lower limit.

            The default value is Size::none(), i.e. there is no minimum for
           either width or height.

            The view will automatically apply this constraint when calculating
           its preferred size. It is a hard limit, so the view will never report
           a preferred size below this minimum.

            IMPORTANT: this property only influences the preferred size that the
           view requests during layout (see calcPreferredSize()). Its parent
           view may decide to make it bigger than this because of other layout
           considerations.
        */
        BDN_VIEW_PROPERTY(Size, preferredSizeMinimum, setPreferredSizeMinimum,
                          IViewCore, influencesPreferredSize());

        /** An optional upper limit for the preferred size of the view (in DIP
           units). This can be used by the application to influence the layout
           of the view and enforce special sizing.

            Width and/or height of the constraint can be set to
           Size::componentNone() to indicate that the corresponding component
           should not have an upper limit.

            The default value is Size::none(), i.e. there is no maximum for
           either width or height.

            The view will automatically apply this constraint when calculating
           its preferred size. It is a hard limit, so the view will never report
           a preferred size that exceeds this maximum.

            IMPORTANT: this property only influences the preferred size that the
           view requests during layout (see calcPreferredSize()). Its parent
           view may decide to make it bigger than this because of other layout
           considerations.
        */
        BDN_VIEW_PROPERTY(Size, preferredSizeMaximum, setPreferredSizeMaximum,
                          IViewCore, influencesPreferredSize());

        /** Converts a UiLength object to DIPs.
            DIP stands for "device independent pixel", a special unit (see \ref
           dip.md).

            This uses view-specific internal data, so the result can be
           different for different view objects. The result can differ when this
           function is called again at a later time with the same view object
            (if the view's parameters or the operating systems settings have
           changed).
            */
        double uiLengthToDips(const UiLength &length) const;

        /** Converts a UiMargin object to a DIP based margin object.
            DIP stands for "device independent pixel", a special unit (see \ref
           dip.md)

            This uses view-specific internal data, so the result can be
           different for different view objects. The result can differ when this
           function is called again at a later time with the same view object
            (if the view's parameters or the operating systems settings have
           changed).
            */
        Margin uiMarginToDipMargin(const UiMargin &uiMargin) const;

        /** Asks the view to calculate its preferred size in DIPs (see \ref
           dip.md), based on it current contents and properties.

            Note that the View object will cache the result of the call. Calling
           this multiple times with the same availableSpace parameter is a fast
           operation.

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
            -------------------

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


            Important Notes
            ---------------

            IMPORTANT: It is perfectly ok (even recommended) for the view to
           return a preferred size that is not adjusted for the properties of
           the current display / monitor yet. I.e. it may not be rounded to full
           physical pixels yet. The size will be adapted to the display
           properties in adjustAndSetBounds().
            */
        virtual Size
        calcPreferredSize(const Size &availableSpace = Size::none()) const;

      protected:
        /** This is called when the sizing information of a child view has
           changed. Usually this will prompt this view (the parent view) to also
           schedule an update to
            its own sizing information and an update to its layout.*/
        virtual void childSizingInfoInvalidated(View *pChild);

        // allow the coordinator to call the sizing info and layout functions.
        friend class LayoutCoordinator;

        class Influences_
        {
          public:
            Influences_(View *pView) : _pView(pView) {}

            /** Dummy function that does nothing. A call to this can be made in
               the influence section of \ref BDN_VIEW_PROPERTY. */
            const Influences_ &influencesNothing() const { return *this; }

            /** Call this in BDN_VIEW_PROPERTY when the property change
                influences the view's preferredSize (and as such it can also
               influence the parent layout)*/
            const Influences_ &influencesPreferredSize() const
            {
                // update the sizing information. If that changes then the
                // parent layout will automatically be updated.
                _pView->invalidateSizingInfo(
                    InvalidateReason::standardPropertyChanged);

                return *this;
            }

            /** Call this in BDN_VIEW_PROPERTY when the property change
             influences how the view lays out its own children / content.*/
            const Influences_ &influencesContentLayout() const
            {
                // the layout of our children is influenced by this
                _pView->needLayout(InvalidateReason::standardPropertyChanged);

                return *this;
            }

            /** Call this in BDN_VIEW_PROPERTY when the property change
             influences the size of the view's parent, but not the view's own
             preferred size.
             An example of a property with this influence would be the view's
             margin.*/
            const Influences_ &influencesParentPreferredSize() const
            {
                P<View> pParent = _pView->getParentView();
                if (pParent != nullptr)
                    pParent->invalidateSizingInfo(
                        InvalidateReason::standardChildPropertyChanged);

                return *this;
            }

            /** Call this in BDN_VIEW_PROPERTY when the property change
             influences how the view is arranged within the parent, but it does
             not influence the view's own PREFERRED size. Note that the property
             may influence the actual size
             that the parent assigns to the view, based on the arrangement
             values.*/
            const Influences_ &influencesParentLayout() const
            {
                P<View> pParent = _pView->getParentView();
                if (pParent != nullptr)
                    pParent->needLayout(
                        InvalidateReason::standardChildPropertyChanged);

                return *this;
            }

          private:
            View *_pView;
        };

        /** (Re-)initializes the core object of the view. If a core object
           existed before then the old object is destroyed.

            The core object is immediately detached from the view (before
           reinitCore returns).

            If the view is part of a UI hierarchy that is connected to a top
           level window then a new core will be created shortly thereafter.

            A new core is immediately created and attached, before reinitCore
           returns.

            reinitCore also causes the reinitialization of the cores of all
           child views.
            */
        void reinitCore();

        /** Determines the Ui provider to use with this view object.
            The default implementation returns the parent view's Ui provider,
            or null if the view does not have a parent or the parent does not
            have a ui provider.
            */
        virtual P<IUiProvider>
        determineUiProvider(P<View> pParentView = nullptr)
        {
            if (pParentView == nullptr)
                pParentView = getParentView();

            return (pParentView != nullptr) ? pParentView->getUiProvider()
                                            : nullptr;
        }

      private:
        /** Should not be called directly. Use setParentView() instead.
         */
        bool _canMoveToParentView(P<View> pParentView);

        /** Should not be called directly. Use reinitCore() instead.
         */
        void _deinitCore();

        /** Should not be called directly. Use reinitCore() instead.*/
        void _initCore();

      protected:
        P<IUiProvider> _pUiProvider;

        void deleteThis() override;

      private:
        WeakP<View> _parentViewWeak = nullptr;
        P<IViewCore> _pCore;

        mutable PreferredViewSizeManager _preferredSizeManager;
    };
}

#include <bdn/IUiProvider.h>

#endif
