DIP: Unit for user interface and layout              {#dip-units}
=======================================

The default unit in Boden for values that refer to user interface layout, sizes and coordinates is DIP.

DIP stands for "device independent pixel". It is a unit that is roughly the "same perceived size"
on all platforms and devices. What exactly that means is defined below.

The problem
-----------

Distances and sizes for UI elements are surprisingly difficult to specify in a way that works
well on all devices and in all situations.

Physical device pixels are very inadequate because the pixels on high resolution screens are much smaller than those
on low-resolution screens. So if you were to specify
the space between two elements in pixels then the distance might look good on one display, but would
be much too small or too big on other displays.

Another consideration are viewing distances. One does not usually want UI elements to be the same physical size on a phone as they are
on a monitor. Phones are usually held at a shorter viewing distance than monitors, so UI elements can be physically
smaller and still be perfectly readable and usable.
On the other extreme, consider a huge display in a football stadium. The viewing distance is much longer
than on a normal desktop monitor, so texts and UI elements must also be a lot bigger.

Another point for consideration are user preferences. Visually impaired users might need UI elements to be bigger
than users with perfect eyesight. So UI elements should also scale with user preferences.

So these are the main problems when defining UI lengths:

- Different pixel densities / pixel sizes (high resolution displays vs. low resolution displays)
- Different viewing distances (phone vs. monitor vs TV vs stadium screen).
- Different user preferences


The solution
------------

One common solution for the problems is to define a unit that has the same "perceived size" on all devices and platforms.
That way one can define layouts and View properties without needing to know the specifics of the device and platform.
This unit is called the "device independent pixel" or DIP.

An oversimplified definition for this unit would be "the perceived size a pixel would have on an old desktop monitor
from around the years 1995-2005". Common vertical resolutions during that time were between 800 and 1200 pixels and monitors were quite
small by today's standards. We use this rough legacy size is the basic unit for UI elements.
That way old pixel-based measurements can be transferred to high-resolution environments without requiring any conversion.

Note that this definition is similar to the definition of the "px" unit in HTML/CSS. In Boden we have chosen a different name for the unit
to make it clear that DIPs are not physical device pixels.

How many actual device pixels correspond to 1 DIP is different on each platform and on different devices.
It depends on properties like the expected viewing distance of the device, the physical device resolution and even user preferences.

Boden derives the size of 1 DIP from platform dependent properties. Most modern platforms already some concept of a DIP-like
unit or define UI scale factors that serve a similar purpose. Where possible, Boden uses these platform properties as the basis
for the DIP unit.

For example, on Microsoft Windows the size of 1 DIP is derived on the UI scaling factor for the screen,
which in turn depends on the screen's hardware DPI, the screen resolution, the expected viewing distance and user preferences.




