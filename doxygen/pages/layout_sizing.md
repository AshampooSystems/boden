Layout Sizing and UI Space Allocation
===============================================

The Boden layout containers use some generic values from each view to allocate the available space. There
are obviously differences in how each view container arranges its children, but the sizing follows some generic
guidelines.


Preferred size
--------------

Each view reports its preferred size in its View::sizingInfo() property. The preferred size is just that:
the size that the view would prefer to have. The parent containers will try to assign it this size if enough
space is available.

Growing beyond the preferred size
---------------------------------

If more space is available than needed for all the child views then parent containers can make some children
bigger. This is controlled by two properties: the grow priority and the grow factor.

Views with a higher grow priority get a chance to use all available space if they want. Views can only grow
bigger than their preferred size if there are no higher priority views or if those higher priority views cannot
use up all available space (for example because they have maximum size limits - see below). Note that priorities
are relatively rarely used, although they are sometimes necessary to achieve the intended result. The grow factor
is a much more common tool to distribute space. Most views start out with the same default priority.

If multiple views have the same grow priority then the available space is distributed among them according to
the relation of their grow factors. A grow factor that is twice as big as that of another child view
means that the view gets twice the amount of additional space. A grow factor of 0 means that a view cannot
grow beyond the preferred size.

Shrinking below the preferred size
----------------------------------

It sometimes happens that there is not enough space available to give all child views their preferred size. In this
case they can end up smaller than that size. This shrinking is controlled with a similar mechanism as the growing:
there is a shrink priority and a shrink factor.

The algorithm works the same way as the growing algorithm: views with a bigger shrink priority are shrunk first
(potentially down to their minimum size). If this is not enough to fit into the available space then the next lower
priority views are shrunk and so on.

Views with the same priority are shrunk according to how their shrink factor values relate to each other: a shrink
factor that is twice as big as that of another view means that the view will be shrunk twice as much as the other
view (up to the point where it hits is minimum size).

Constraints
-----------

You can set manual constraints for some automatically calculated values to force them to a specific value.
For example, you can restrict the preferred size that a view reports with the two properties
\ref bdn::View::preferredSizeMinimum() and \ref bdn::View::preferredSizeMaximum(). You if you set the
maximum and minimum constraints to the same value then you can also force the view to report a specific preferred
size, thus completely overriding the internal automatic sizing.

There are also constraints to the final actual size of the view (\ref bdn::View::sizeMinimum() and
\ref bdn::View::sizeMaximum()). These constraints are applied at the end of the sizing process.

All these constraints are properties, so they can be bound to values of other properties. So one can, for example,
bind the maximum size constraint of one view to the actual size of another view. That enforces that the first view
can be at most as big as the other view.

Examples
--------

So, how should grow and shrink priorities and factors be used?

While the priorities can be manually modified by the app this is rarely necessary. Some view types automatically start
out with a lower default priority to achieve the correct sizing in most circumstances. For example, ScrollViews
automatically have a higher shrink priority than normal views in the direction that they can scroll. That is
because they can deal with less space easily in the scroll directions, so it is usually preferable to reduce a
scrollable view rather than a normal view. The default priorities work well in most cases and usually do not
need to be modified.

Grow and shrink factors on the other hand are frequently used parameters. For example, if you have a row in your
UI with a text label and a text edit field then you usually want the label to keep its preferred size and the
may want the edit field to use up all available space. You would assign a grow factor of 0 to the label and 
some factor >0 (for example, 1) to the edit field. The actual value of the edit field's grow factor does not
matter in this example, as long as it is >0. That is because there are no other views in the row, so any value >0
will cause the control to use up all available space.

If you wanted to have two views in a container and both should use up equal amounts of free space then you would
assign both of them the same grow factor. It must be >0 and it must be the same for both views, other than that
the actual value does not matter in this case. You could set both to 1, or both to 123.

If you wanted to have one control use up twice as much of the free space as the other then you could assign
one a grow factor of 1 and the other a grow factor of 2.

Note that grow factors only control how additional free space is assigned. Equal grow factors do not necessarily
mean that two controls will have the same size in the end. That would only be true if they also had the same
initial preferred size.
If you want two controls to have the same size then it is best to link their preferred sizes by binding
their \ref bdn::View::preferredSizeMinimum() and \ref bdn::View::preferredSizeMaximum() constraint properties
to the \ref bdn::View::preferredSize() property of the other view (using \ref bdn::Property::bind()).









