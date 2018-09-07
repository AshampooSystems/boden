Layout Box Model
================

Boden uses a layout model that closely resembles the box model used in the HTML and CSS standards.
Note that while the general setup is similar, there are some differences in the terminology.
In particular, the term "client area" is defined a little differently in Boden compared to HTML/CSS.

The general structure of the box model looks like this:

    ---------------------------------
    |            margin             |
    |  -------view boundary-------  |
    |  |     non-client area     |  |
    |  |  -----client area-----  |  | 
    |  |  |      padding      |  |  | 
    |  |  |  --content area-  |  |  |
    |  |  |  |   content   |  |  |  |
    |  |  |  ---------------  |  |  |
    |  |  |                   |  |  |
    |  |  ---------------------  |  |
    |  |                         |  |
    |  ---------------------------  |
    |                               |
    ---------------------------------


Content area
------------

This is the area where the view's content and/or child views are displayed.


Padding
-------

The padding value indicates how much empty space there should be around the
content area.


Client area
-----------

The client area is the content area plus the padding.

If the view supports scrolling then the term client area refers to the scrolled area
inside the view. So the client area can actually be bigger than the view itself in such cases.


Client coordinates
------------------

Client coordinates are relative to the top-left corner of the client area. Note that
client coordinates like all other layout values are always specified in DIP units
(see [dip.md][DIP Unit])


Viewport
--------

If a view supports scrolling then the viewport is the part of the view that shows the
visible portion of the client area.

Note that scroll bars are NOT part of the viewport.

If a view does not scroll then the viewport is the same as the client area.


Non-client area
---------------

Some views have other elements around the client area / viewport.
For example, header panels, a border, scroll bars or similar. These elements are called
the non-client parts and the space they take up is called the non-client area.


Margin 
------

A view's margin indicates how much empty space there should be around the view.
The margin is not part of the view's area - it only influences how the view 
and its sibling views are arranged inside its parent.










