#ifndef BDN_UiLength_H_
#define BDN_UiLength_H_


namespace bdn
{
	
/** Represents a length or distance to use sizing and spacing user interface elements.

    UiLength objects can also be "none", which means that they represent a non-existent value
    (similar to the standard nullptr value). Default-constructed UiLength objects are none.



    
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

            See \ref dip.md for more information.
			
            DIP is the default unit for Boden.
			*/
		dip,

        
        /** 1 sem equals the height of the screen's default UI font.
		
			Use this when you want a length to depend on the default text size.            
			*/
		sem,	
    
        
        /** 1 em is the height of the active font. Which font this is depends on where the
            UiLength element is used. For example, if the UiLength object is assigned to a View object then this refers to
            the font of the view.

            Use this when you want a length to scale with the text size of the UI element.
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


	/** Returns a locale-independent string representation of the UI length.
		*/
	String toString() const
	{
		if(unit==Unit::none)
			return "none";
		else
			return bdn::toString(value)+" "+unitToString(unit);
	}
	
	Unit	unit;	
	double	value;	


private:
	static String unitToString(Unit unit)
	{
		if(unit==Unit::dip)
			return "dip";
		else if(unit==Unit::sem)
			return "sem";
		else if(unit==Unit::em)
			return "em";
		else
			return "";
	}
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


