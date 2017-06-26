Layout architecture for the WinUWP target platform
==================================================

This is an internal development article which describes the rationale behind the way we arrange and size
views on the Universal Windows Platform.

How the UWP layout system works
-------------------------------

UWP views provide two functions: Measure and Arrange. Measure calculates the desired size of the view.
The result is cached in the DesiredSize property.
Arrange arranges child views. Usually the children are sized according to their DesiredSize property, i.e.
the result of the last Measure call.

There are some special cases for this process. For one thing, Measure has a parameter called availableSize,
which indicates how much space the parent has assigned to the child view. The child view can then return
a DesiredSize that tries to fit into that space (possibly adjusting the internal content layout, like breaking
text into lines). Even in availableSize is set, this still sets the normal DesiredSize property.

So, DesiredSize depends on the input parameter to the last Measure call. And the documentation states that
Arrange should use that property to size the child. If one does it that way then this implicitly assumes
that the availableSize parameter of the latest Measure call accurately reflects the available space in Arrange.

And that can be a problem, since Arrange also has an "available size" parameter (called finalSize). It is perfectly
ok for that finalSize to not match the last availableSize parameter, especially in cases when there is not enough
space to give every panel their desired size. So final layout depends on a property (DesiredSize) that may have been calculated
for a completely different availableSize. That means that the child cannot adjust properly to the actual final available
space.

This problem occurs when one follows the recommended layout procedure. Of course, one could call Measure for the child
views in Arrange if the finalSize in Arrange does not match the last availableSize from Measure. This would yield
the correct layout, but it does not conform to the recommended approach.


Arranging and sizing UWP views
------------------------------

There are two ways to provide a custom layout in UWP: to provide a custom panel class which overrides the layout
functions, or to use a Canvas and arrange the child views by setting their properties.

Canvas has custom properties to define the desired position of the child views. The size of the child views
is the view's preferred size by default. It can be overridden by setting the Width and Height property. The Width
and Height property influence the child's desired that it reports and thus the end result is usually that the view
ends up with the size defined by Width and Height.

But setting Width and Height for size control has three problems:

1) They influence the result of Measure. So one cannot get accurate desired size information from the control
   when Width and Height are set.

2) Changing them invalidates the child's cached measurements and triggers a re-layout of the parent. So temporarily
   clearing the values for measuring can cause additional layout cycles, and even endless layout loops.

3) The actual size used is still the result of the child's Measure operation. The Width and Height properties are intended to
   represent only a guideline for the final measured size, so the view is free to ignore them. That means that
   the actual size of the controls might end up being something else.

So, using Width and Height is out of the question. If we want to use our own custom size calculations then we have to
overload Measure on the child controls and ensure that Measure returns the size we want them to have. That in turn
would mean that we have to subclass all controls and override their measure function and we could never
use controls that were already created at runtime. That is a pretty harsh restriction, which we do not have.

So, the "magic" has to happen in the Arrange function of the layout container. That function has to assign the actual
size that we want the view to have and pretty much ignore the DesiredSize property.

So we have to use custom panels with overloaded layout function.
There we have to overload MeasureOverride to provide our own desired size. And we have to overload ArrangeOverride
to arrange and size the controls according to our own layout routines.


Layout coordination
-------------------

We now know how we want to size and arrange the controls, but we still have to decide WHEN we want to do this.

The Universal Windows platform has its own layout coordinator. The Windows runtime decides when to update
the layout and when to arrange the views. One can manually schedule a re-layout or re-sizing but Windows determines
when to actually do this.

Boden also has a layout coordinator, that makes these decisions based on the custom Boden logic.

### Using Boden's layout coordinator

In the interest of behaving consistently across all platforms it would be nice to completely replace the Windows
layout coordination with the standard Boden LayoutCoordinator.

However, there are some challenges in doing it that way.

For one thing, the Arrange method only works when called from within a Windows layout cycle. So we always have to
ensure that a Windows layout cycle follows our own layout and until then our new layout will not take effect.

Also, the Measure method, which calculates the desized size of the control, may not return up-to-date information
when called from outside the layout cycle. So we have to ensure that we do not do our own layout too early, or we have
to update the layout after each Windows layout cycle.

This is what we need to do:

- When the container's Measure method is called we need to invalidate the preferred size of the container and of the
  children. The update will happen asynchronously. We do not measure anything here and simply return the old desired size.

- When the container's Arrange method is called we need to arrange the children according to the bounds of Boden's View
  objects.

- When the sizing update happens we will update the view's peferred size property. This will cause our parent layout
  to be invalidated.

- When our own layout happens we update the Boden view's size and position properties. Then we need to invalidate the
  Windows layout, causing Arrange to be called again, to make these changes active.

So, all in all the Windows Arrange pass may happen twice when the layout changes, depending on wether or not Windows
does it before our own layout is done. However, it is very fast, since Arrange only applies the precalculated
layout - so that should be ok.

### Using the Windows layout coordinator

We could also simply do the sizing and layout on demand when Windows calls our Layout and Arrange routines.
It seems that the Windows layout system is quite reliable with these calls and they are also optimized to reduce
duplicate operations to a minimum.

- needLayout and needSizingInfoUpdate simply call Windows InvalidateMeasure and InvalidateArrange methods

- In Measure we update our own preferredSize property

- In Arrange we call layout. Note that this will call Measure again on the child views, but this is actually
  necessary to ensure that the layout is correct (since finalSize from Arrange does not necessarily match
  the availableSize from Measure). This should not cause any problems, even though it invalidates the child's
  layout - the child always needs to be re-layouted after its parent, since its final size is not known
  before that.

We could also optimize this even more by calculating our full layout in Measure and then simply
applying it in arrange if finalSize matches availableSize. However, since that cannot be guaranteed, this
additional complexity does not seem to be worth it.


### Layout coordinator choice

We use the Windows layout coordinator. While it might cause some inconsistencies with other platforms, it
makes us a better behaved citizen in the Windows ecosystem and will likely reduce strange bugs and race conditions.
It should also be faster.


Container Views
---------------

The boden container views are custom panels that override the layout functionality.

When the boden layout system does a layout cycle it eventually calls View::adjustAndSetBounds on the child views.
The child view should update its size and position in that call. The view core will simply caches this information
and not update the UWP view at that point in time.
The actual update happens asynchronously in the background, soon after the change.

Scroll Views
------------

We want to use the normal ScrollViewer implementation of UWP for our scroll views. The ScrollViewer
automatically handles the sizing and arranging of its content view, so we cannot do this ourselves like we did with the
Panel-derived container views.

We add a custom panel as the content view of the ScrollViewer. This panel acts as a bridge between the Windows ScrollViewer
and the Boden content view. It ensures that we report the correct desired size to the scroll viewer, so that the scrolling
functionality is updated correctly.






