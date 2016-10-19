#ifndef BDN_UiLength_H_
#define BDN_UiLength_H_


namespace bdn
{
	
/** Represents a length or distance to use sizing and spacing user interface elements.

    UiLength objects can also be "none", which means that they represent a non-existent value
    (similar to the standard nullptr value). Default-constructed UiLength objects are none.

    Which unit to use
    =================

    DIP (UiLength::Unit::dip) can be considered the default unit. Use this when none of the special considerations apply.

    Use UiLength::Unit::sem when you want to specify a length relative to the text size.

    Use UiLength::Unit::em when you want the length to grow/shrink when there is a bigger font selected for the UI element.

    See below for more information on the units.

    
	The units
	=========

	Distances and sizes for UI elements are surprisingly difficult to specify in a way that works
	well on all devices and in all situations.

	Pixels are very inadequate because the pixels on high resolution screens are much smaller than those
    on low-resolution screens. So if you were to specify
	the space between two elements in pixels then the distance might look good on one display, but would
	be much too small or too big on other displays.

	Another consideration are viewing distances. You do not want UI elements to be the same physical size on a phone as they are
	on a monitor. Phones are usually held at a shorter viewing distance than monitors, so UI elements can be physically
	smaller and still be perfectly readable and usable.
	On the other extreme, consider a huge display in a football stadium. The viewing distance for people is much longer
	than on a normal desktop monitor, so texts and UI elements must also be a lot bigger.
	
	Another point for consideration are user preferences. Visually impaired users might need UI elements to be bigger
	than users with perfect eyesight. So UI elements should also scale with user preferences.

	So these are the main problems when defining UI lengths:

	- Different pixel densities / pixel sizes (high resolution displays vs. low resolution displays)
	- Different viewing distances (phone vs. monitor vs TV vs stadium screen).
	- Different user preferences


    The DIP ("device independent pixel") unit (UiLength::Unit::dip)
	---------------------------------------------------------

	One wide-spread solution to the problems is to define the unit as the size of a "legacy pixel" on an old desktop monitor
    from the times when most monitors had similar pixel sizes. This is often called a "device independent pixel", or DIP in short.

	An oversimplified definition for this unit would be "the perceived size a pixel would have on an old desktop monitor",
    around the years 1995-2005. Common vertical resolutions during that time were between 800 and 1200 pixels and monitors were quite
    small by today's standards.
 
    The idea behind the new DIP unit is to use this rough legacy size as the basic unit for UI elements.
    That way old pixel-based measurements can be transferred to high-resolution environments without requiring any conversion.

	Note that DIPs / "device independent pixels" do not refer to actual physical pixels. The physical pixels are usually
    much smaller than 1 DIP.
    
    How 1 DIP maps to actual device pixels depends on many variables, like the expected viewing distance of the device and even user preferences.
    For example, on Windows
    the size of this unit depends only on the UI scaling factor for the screen, which in turn depends on the screen's hardware DPI,
    the resolution, the expected viewing distance and also user preferences.


	The sem unit  (UiLength::Unit::sem)
	-----------------------------
	
	Another unit that solves all of the problems described above is called "sem". 1 sem equals the height of the default system font
	that the operating system recommends for the screen that the UI element is currently on.

	Why does sem solve all the problems described above?

	On a desktop monitor, the operating system and user will choose a system font size that is comfortably readable
	on the monitor in use. Multi-monitor systems with different monitor resolutions will also usually use different
	font sizes for each monitor, so that text on each monitor is readable comfortably.

	If the user has a visual impairment then he or she will adjust the operating system settings to increase the default
	font size.

	On different device types (phones, tablets, TVs and huge stadium screens) the default font size is adapated
    to the expected viewing distance of the user. Phone and tablet font sizes are smaller, TV fonts are bigger
	and stadium screen font sizes are huge.

	The term "sem" is derived from the term "em" (commonly used in CSS, for example), which denotes a size relative to a font size.
	The leading "s" is added because the font in question is the default screen font, not any custom font that might be selected for an element.

	Note that 1 sem is actually a rather large size, compared to a pixel or a DIP. It is the height of the font. So some distances will actually
	commonly be specified as fractional values. For example, the space between two elements might be 1.5 sem or something similar.

    
    The em unit (UiLength::Unit::em)
    --------------------------

    "em" is a unit that depends on the properties of the UI element that it is used with.
    1 em equals the font height of the font that is used by the UI element. So 1 em can have a different size for different
    elements if they have different fonts.

    Since the font sizes used by UI elements are usually derived in some way from the default system font size, the em unit
    also solves the problems described above. The reasoning is the same as for the sem unit.

    The major difference for em is that it scales with the font size of the UI element. Use this when you want a length to
    depend on the actual font sizes used. For example, on might use em for button padding to make the padding sizes bigger when
    the button has a bigger font.


    
	*/
struct UiLength : public Base
{
public:
	enum class Unit
	{
        /** Special unit that indicates a "null" length. I.e. a UiLength object that has no value.
        
            The value variable is ignored if the unit is none.
        */
        none,


        /** A "device independent pixel". This unit corresponds roughly to the perceived size of a pixel on
            an legacy 96 dpi desktop monitor.
			
			Note that this unit actually takes other factors into account as well (expected viewing distance of the device,
			user preferences, etc.). See the UiLength class documentation for more information.

            DIP can be considered the default unit.
			*/
		dip,

        
        /** 1 sem equals the height of the screen's default UI font.
		
			Use this when you want a length to depend on the default text size.

			See the UiLength class documentation for more information.
			*/
		sem,	
    
        
        /** 1 em is the height of the active font. Which font this is depends on where the
            UiLength element is used. For example, if the UiLength object is assigned to a View object then this refers to
            the font of the view.

            Use this when you want a length to scale with the text size of the UI element.

            See the UiLength class documentation for more information.
            */
        em
    
    };


    /** Default constructor - sets the unit to #UiLength::Unit::none and value to 0.*/
	UiLength()
	{
		unit = UiLength::Unit::none;
		value = 0;		
	};

    UiLength(double value, Unit unit = UiLength::Unit::dip)
	{
		this->value = value;
        this->unit = unit;
	}


    /** Creates a UiLength object with the specified value and the UiLength::Unit::sem unit.*/
    static UiLength sem(double value)
    {
        return UiLength( value, UiLength::Unit::sem );
    }

    /** Creates a UiLength object with the specified value and the UiLength::Unit::em unit.*/
    static UiLength em(double value)
    {
        return UiLength( value, UiLength::Unit::em );
    }


    /** Creates a UiLength object with the specified value and the UiLength::Unit::dip unit.*/
    static UiLength dip(double value)
    {
        return UiLength( value, UiLength::Unit::dip );
    }


    /** Creates a UiLength object with the UiLength::Unit::none unit (same as default-constructed UiLength).*/
    static UiLength none()
    {
        return UiLength();
    }



    /** Returns true if this UiLength object has the special "none" value.*/
    bool isNone() const
    {
        return (unit==UiLength::Unit::none);
    }
	
	Unit	unit;	
	double	value;	
};


}


inline bool operator==(const bdn::UiLength& a, const bdn::UiLength& b)
{
	return (a.unit==b.unit
			&& a.value==b.value);
}

inline bool operator!=(const bdn::UiLength& a, const bdn::UiLength& b)
{
	return !operator==(a, b);
}


#endif


