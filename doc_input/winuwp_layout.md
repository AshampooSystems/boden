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

### DesiredSize and available space

Measure has a parameter called availableSize, which indicates how much space the parent has assigned to the child view. The child view can then return
a DesiredSize that tries to fit into that space (possibly adjusting the internal content layout, like breaking
text into lines). The DesiredSize property is initialized with the last Measure result, whether availableSize
was set or not.

### DesiredSize result restricts final size

There are some restrictions for the final size a control can get in Arrange. A UI element can never be assigned
a size that is smaller than its DesiredSize. When a smaller size is passed to the element's Arrange function
then the element ends up having its DesiredSize. However, a view can be made bigger than its desired size.
Note that this behaviour of DesiredSize being a lower limit for the final size is not documented. In fact,
the documentation states explicitly that the parent container can ignore the DesiredSize and another size.
So the documentation is - as of 2017-07-03 - incorrect, since only bigger sizes can be used.

This restriction does make sense in a way. It forces the parent container to call Measure again with a smaller
availableSpace if it cannot deliver the original DesiredSize. So this rule ensures that each control gets
a chance to adapt to a smaller size.


### Do not call Measure from Arrange

One more caveat is that Measure invalidates Arrange information. I.e. calling Measure will initiate
a new layout cycle. That means that one should usually not call Measure from within Arrange, otherwise
a layout cycle might be detected by Windows and a error might be raised.


### Example: ScrollViewer

ScrollViewer is a good example for the DesiredSize restriction. If it's Measure function is called
with an infinity availableSize then the scroll viewer will report a DesiredSize that matches its
content size. And since it cannot be made smaller than that in Arrange that means that
the scroll viewer will never scroll.

### Calculating the layout in Measure

Since the last Measure result imposes such restrictions on the sizes, that basically means that all sizes have to
be finalized in Measure. Otherwise controls whose width and height are interdependent (like TextBlock)
might not be sized correctly.

In general, it is most helpful to think of a container's Measure call as a function that calculates the actual
final sizes of the children - i.e. it basically calculates the whole layout. Arrange then only sets the positions
and activates the layout from the last Measure call.

This simplification is not 100% correct, theoretically. The final size a container gets in its own Arrange call
might be bigger than the DesiredSize it reported. In that case it would not be ok to calculate the full layout
in Measure and then activate it in Arrange: the container would end up not using the additional space.

However, if a bigger size is to be assigned to the container in Arrange then the container must get the opportunity
to resize its children. And making the children larger can also influence their DesiredSize - they might want
to get wider but less high, for example. That is especially the case if the last availableSpace from Measure was
too small and some of the children had to restrict their size.

So, we have established that the whole system does not work correctly if containers get assigned a larger size in Arrange.

Luckily, the application has almost full control over whether that happens. The top level measure call will always get the Window
size as its availableSpace and that won't change during the layout process. So if all containers in the tree
make sure that they do not make controls larger than the availableSpace from the last Measure call then this bad
behaviour can never occur. In that case it is ok to calculate the full layout in Measure.


Measuring views outside the layout cycle
----------------------------------------

As mentioned above, the result of Measure imposes restrictions on the size a control can have. Thus it
is important that it has the correct value when Arrange is called.

What happens when our own calcPreferredSize method is called at other times? calcPreferredSize obviously
has to call Measure, thus it will overwrite the cached DesiredSize. A Measure call also invalidates the arrange,
information, so an Arrange call will follow automatically sometime after that. So the last measure result
will become active automatically and modify the layout.

So when calcPreferredSize is called outside the layout cycle we have to make sure that we restore the
previous DesiredSize. We can do that by caching the availableSpace parameter from the last layout Measure and
then calling Measure again with that after the non-layout Measure was done. It is inefficient, but
apparently there is no way around it.


Canvas or Panel?
----------------

There are two ways to provide a custom layout in UWP: to provide a custom panel class which overrides the layout
functions, or to use a Canvas and arrange the child views by setting their properties.

Canvas has custom properties to define the desired position of the child views. The size of the child views
is the view's DesiredSize by default. It can be overridden by setting the Width and Height property. The Width
and Height property influence the child's desiredSize and thus the end result is usually that the view
ends up with the size defined by Width and Height.

But setting Width and Height for size control has three problems:

1) They influence the result of Measure. So one cannot get accurate desired size information from the control
   when Width and Height are already set.

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
size that we want the view to have.

So we have to use custom panels with an overloaded Arrange function.
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
  children. The preferred size will be updated asynchronously. We do not measure anything in Measure
  and simply return the old desired size.

- When the container's Arrange method is called we need to arrange the children according to the bounds of Boden's View
  objects.

- When the Boden sizing update happens we will update the view's peferred size property. This will cause our parent layout
  to be invalidated.

- When our own layout happens we update the Boden view's size and position properties. Then we need to invalidate the
  Windows layout, causing Measure and Arrange to be called again, to make these changes active.

So, all in all the Windows Arrange pass may happen twice when the layout changes, depending on wether or not Windows
does it before our own layout is done. However, it is very fast, since Arrange only applies the precalculated
layout - so that should be ok.

### Using the Windows layout coordinator

We could also simply do the sizing and layout on demand when Windows calls our Layout and Arrange routines.
It seems that the Windows layout system is quite reliable with these calls and they are also optimized to reduce
duplicate operations to a minimum.

However, there are several restrictions:

1) One must never call Measure (=calcPreferredSize) from inside the Arrange
   routine (see above).

2) One basically has to assign the final size of the views in Measure (since Measure sets up restrictions for
   the final size.

Since Boden allows layout containers to call calcPreferredSize during layout, and calcPreferredSize has to call
Measure, the first restriction means that the Boden layout phase cannot happen in Arrange.
Instead, it has to be done in Measure.

The second restriction also means that we have to do the whole layout in Measure.

The good news is that the restrictions Measure places on the final size of the control pretty much enforce that
the availableSpace from the last Measure call reflects the final size of the control. Since we use our own layout
containers we can also enforce that fact. So that means that we can simply calculate the final layout in Measure
and then simply activate it in Arrange.

So, in summary:

- needLayout and needSizingInfoUpdate simply call Windows InvalidateMeasure and InvalidateArrange methods

- In Measure we update our own preferredSize property. Containers calculate the whole layout there
  and store it in a cache.

- In Arrange we simply arrange the UWP controls according to the precached layout



### Layout coordinator choice

We use the Windows layout coordinator. While it might cause some inconsistencies with other platforms, it
makes us a better behaved citizen in the Windows ecosystem and will likely reduce strange bugs and race conditions.
It should also be faster.






