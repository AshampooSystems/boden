#ifndef BDN_IViewCore_H_
#define BDN_IViewCore_H_

namespace bdn
{
    class IViewCore;
}

#include <bdn/UiMargin.h>
#include <bdn/Rect.h>
#include <bdn/Nullable.h>
#include <bdn/Size.h>
#include <bdn/View.h>


namespace bdn
{

class IViewCore : BDN_IMPLEMENTS IBase
{
public:


	/** Shows/hides the view core.*/
	virtual void setVisible(const bool& visible)=0;
	
	/** Sets the view core's padding. See View::padding() */
	virtual void setPadding(const Nullable<UiMargin>& padding)=0;


    


    /** Sets the view's position and size, after adjusting the specified values
        to ones that are compatible with the underlying view implementation. The bounds are specified in DIP units
        and refer to the parent view's coordinate system.
        
        See adjustBounds() for more information about the adjustments that are made.
        
        Note that the adjustments are made with a "nearest valid" policy. I.e. the position and size are set
        to the closest valid value. This can mean that the view ends up being bigger or smaller than requested.
        If you need more control over which way the adjustments are made then you should pre-adjust the bounds
        with adjustBounds().

        The function returns the adjusted bounds that are actually used.
        */
    virtual Rect adjustAndSetBounds(const Rect& requestedBounds)=0;



    /** Adjusts the specified bounds to values that are compatible with the underlying view implementation
        and returns the result. The bounds are specified in DIP units and refer to the parent view's coordinate system.

        IMPORTANT: This function must only be called from the main thread.

        Not all positions and sizes are necessarily valid for all view implementations. For example,
        the backend might need to round the abstract DIP coordinates to the nearest physical pixel boundary.

        The function adjusts the specified bounds according to its implementation constraints and returns the
        valid values. The positionRoundType and sizeRoundType control in which direction adjustments are made
        (adjusting up, down or to the nearest valid value).        
    */
    virtual Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const=0;



        
	
	/** Converts the specified Ui length to DIPs (device independent pixels  - see UiLength::Unit::dip).
    
        Return 0 for "none" values (see UiLength::isNone()).

    */
	virtual double uiLengthToDips(const UiLength& uiLength) const=0;
	

	/** Converts a UiMargin object to a DIP-based margin object (see UiLength::Unit::dip).*/
	virtual Margin uiMarginToDipMargin(const UiMargin& margin) const=0;



	

	/** Asks the view core to calculate its preferred size in DIPs (see UiLength::Unit::dip),
        based on it current content	and properties.
        
		maxSize is used to indicate the maximum amount of available space for the view (also in DIPs).
        maxSize can equal Size::none(), in which case the available space should be considered to be unlimited.
		I.e. the function should return the view's optimal size.

        It is also possible to have a limit for only one component (width or height). In that case that maxSize component
        will have a normal finite value, while the other unrestricted component is set to Size::componentNone().

        Analogous to maxSize, minSize represents a lower limit for the preferred size.       

        The view core should try to adapt to the specified constraints in the best way possible for it.        
        For example, many views displaying text can handle a limited available width by wrapping the text into
		multiple lines (and thus increasing their height).

		If the view cannot reduce its size to fit into the limits indicated by minSize and maxSize then it is valid for the function
		to return a size that exceeds the available space. However, the layout manager is free to
		size the view to something smaller than the returned preferred size, potentially causing the content to be clipped.

        IMPORTANT: It is perfectly ok (even recommended) for the view to return a preferred size
        that is not adjusted for the properties of the current display / monitor yet. I.e. it may not be rounded
        to full physical pixels yet.
        The returned size may be adjusted to the display when it is actually assigned to the view with adjustAndSetBounds().

		*/	
	virtual Size calcPreferredSize( const Size& minSize = Size::none(),
                                    const Size& maxSize = Size::none() ) const=0;

	


	/** Called when the outer view's parent has changed.
		
		tryChangeParentView should try to move the core over to the new parent.

		If successful then it should return true, otherwise false.

		If false is returned then this will cause the outer view object to
		automatically re-create the core for the new parent and release the current
		core object.		
		*/
	virtual bool tryChangeParentView(View* pNewParent)=0;
    



    
};


}

#include <bdn/View.h>

#endif
