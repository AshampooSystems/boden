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

    /** Requests that the view updates its sizing information (preferred size, etc.).
		The measuring does not happen immediately in this function - it is performed asynchronously.		

        View core implementations may use their platform's native layout system
        to schedule the update. They can also use the generic bdn::LayoutCoordinator class
        or a custom implementation.
		*/
	virtual void needSizingInfoUpdate()=0;


	/** Requests that the view updates the layout of its child view and contents.
		
		The layout operation does not happen immediately in this function - it is performed asynchronously.		

        View core implementations may use their platform's native layout system
        to schedule the update. They can also use the generic bdn::LayoutCoordinator class
        or a custom implementation.
		*/
	virtual void needLayout()=0;



    /** This is called when the sizing information of a child view has changed.
        Usually this will prompt this view (the parent view) to also schedule an update to
        its own sizing information and an update to its layout.*/
    virtual void childSizingInfoChanged(View* pChild)=0;



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



	

	/** Asks the view to calculate its preferred size in DIPs (see UiLength::Unit::dip),
        based on it current contents	and properties.

        There are several constraints for the preferred size:

        availableSpace
        --------------
        
		The availableSpace function parameter is used to indicate the maximum amount of available
		space for the view (also in DIPs). If availableSpace is Size::none() (i.e. width and height equal Size::componentNone())
        then that means that the available space should be considered to be unlimited.
		I.e. the function should return the view's optimal size.

		When one of the availableSpace components (width or height) is not Size::componentNone() then it means that the available space is limited
		in that dimension. The function should return the preferred size of the view within those constraints,
		trying to not exceed the limited size component.
		
		For example, many views displaying text can handle a limited available width by wrapping the text into
		multiple lines (and thus increasing their height).

		If the view cannot reduce its size to fit into the available space then it is valid for the function
		to return a size that exceeds the available space. However, the layout manager is free to
		size the view to something smaller than the returned preferred size.

        preferredSizeHint(), preferredSizeMinimum() and preferredSizeMaximum()
        ---------------------------------------------

        calcPreferredSize must also take the View::preferredSizeHint(), View::preferredSizeMinimum() and View::preferredSizeMaximum()
        properties into account and constrains the result accordingly.
        
        preferredSizeHint() can be used to provide an advisory hint to the view as to what the preferred width and/or height should
        roughly be. The view is free to ignore this, however. Text views often use this to select the place where they wrap their
        text into multiple lines, for example.

        preferredSizeMinimum() and preferredSizeMaximum() are optional hard limits. calcPreferredSize should never
        return a size that violates these limits, if they are set. Even if that means that the view's content does not fit into
        the view.
        
        If there is a conflict between the minimum and maximum and/or hint values then the values should
        be prioritized in this ascending order: hint, minimum, maximum.
        So the maximum value has the highest priority and the returned value should never exceed
        it. For example, if a minimum is set that exceeds the maximum
        then the maximum should "win" and the preferred size should not exceed the maximum.
     

        Important Notes
        ---------------

        IMPORTANT: It is perfectly ok (even recommended) for the view to return a preferred size
        that is not adjusted for the properties of the current display / monitor yet. I.e. it may not be rounded
        to full physical pixels yet. The size will be adapted to the display properties in adjustAndSetBounds().        

        IMPORTANT: This function must only called be called from the main thread.
		*/		
    virtual Size calcPreferredSize( const Size& availableSpace = Size::none() ) const=0;

	


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
