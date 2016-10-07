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



        
	
	/** Converts the specified Ui length to DIPs (device independent pixels  - see UiLength::Unit::dip).*/
	virtual double uiLengthToDips(const UiLength& uiLength) const=0;
	

	/** Converts a UiMargin object to a DIP-based margin object (see UiLength::Unit::dip).*/
	virtual Margin uiMarginToDipMargin(const UiMargin& margin) const=0;



	

	/** Asks the view core to calculate its preferred size in DIPs (see UiLength::Unit::dip),
        based on it current content	and properties.
        
		availableWidth and availableHeight are used to indicate the maximum amount of available
		space for the view (also in DIPs). If they are both -1 then that means that the available space should be considered to be unlimited.
		I.e. the function should return the view's optimal size.

		When one of the availableXYZ parameters is not -1 then it means that the available space is limited
		in that dimension. The function should return the preferred size of the view within those constraints,
		trying to not exceed the limited size component.
		
		For example, many views displaying text can handle a limited available width by wrapping the text into
		multiple lines (and thus increasing their height).

		If the view cannot reduce its size to fit into the available space then it is valid for the function
		to return a size that exceeds the available space. However, the layout manager is free to
		size the view to something smaller than the returned preferred size.

		*/	
	virtual Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const=0;

	


	/** Called when the outer view's parent has changed.
		
		tryChangeParentView should try to move the core over to the new parent.

		If successful then it should return true, otherwise false.

		If false is returned then this will cause the outer view object to
		automatically re-create the core for the new parent and release the current
		core object.		
		*/
	virtual bool tryChangeParentView(View* pNewParent)=0;
    



     
    /** Adjusts the specified view bounding rectangle (in DIP units) for the physical display that the view is
        currently on. The rect is understood to specify a potential size and position for this view, so
        the coordinates refer to the view parent's coordinate space, just like the those used with setPosition() and setSize().

        Most view implementations will round the view position and size to the boundaries of full physical pixels
        of the particular display that the view is currently on. adjustBoundsRectForDisplay will perform adjustments
        like these and also give you some control over how any necessary rounding is performed.

        Normally, the setPosition() and setSize() functions will automatically adjust their parameters for the pixel grid of the current display.
        However, you can use adjustBoundsRectForDisplay to have more control over the rounding process and then pass pre-adjusted values
        to setSize and setPosition.
    */
    virtual Rect adjustBoundsRectForDisplay(const Rect& rect, RoundType positionRoundType, RoundType sizeRoundType ) const=0;


    

    /** Returns the size of a physical pixel in DIP units (DIP = device independent pixel - see UiLength::Unit::dip).
        The number is often not an integer. For example, the function could return 0.2 if there are
        5 physical pixels for each DIP unit.

        If the size of physical pixels cannot be determined by the implementation then it may return a different value
        chosen by the implementation (for example 1). However, since UI element positions and sizes are aligned
        by this value, such a replacement value should be chosen wisely.
        
        The returned value should NOT be stored for later use because it can change at runtime
        (even for the same view object). For example, it can change when the view is moved to a different screen,
        when a view parent changes, when the user changes his monitor settings, and also at other implementation
        specific times. So this should be considered a temporary value.
        */
    virtual double getPhysicalPixelSizeInDips() const=0;
	
};


}

#include <bdn/View.h>

#endif
