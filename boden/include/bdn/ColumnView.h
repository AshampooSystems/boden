#ifndef BDN_ColumnView_H_
#define BDN_ColumnView_H_

#include <bdn/ContainerView.h>

namespace bdn
{
	

/** A container view that arranges its children in a vertical column,
	one below the other. See #RowView for a similar horizontal container.

	The children's View::horizontalAlignment() property controls how 
	the child views are aligned horizontally.

	If there is extra space available in the container after sizing all
	children to their preferred size then the remaining space is distributed
	according to the children's View::extraSpaceWeight() property.	
	
	*/
class ColumnView : public ContainerView
{
public:
	ColumnView()
	{		
	}
	

	Size calcContainerPreferredSize( const Size& availableSpace = Size::none() ) const override;
	
	P<ViewLayout> calcContainerLayout(const Size& containerSize) const override;
	
private:
	
	/** Calculates the positions and sizes (in DIPs - see UILength::Unit::dip) of the child views for the case that the ColumnView
		has the specified width.

		availableSpace indicates the amount of space that is available for the client views inside the parent.

        forMeasuring indicates that the call is not intended for arranging the children, but to measure the preferred size
        of the container. In this case availableSpace is interpreted as in calcPreferredSize as a "recommended upper limit".
		The size limits for preferred sizes (View::preferredSizeMaximum, View::preferredSizeMinimum) are also factored in.
		If forMeasuring is false then availableSpace is interpreted as the fixed predetermined container size        

		If forMeasuring is true then width and/or height of availableSpace can be Size::componentNone(), which
		means that the space is unlimited - i.e. the bounds should be measured for the case when any preferred size is acceptable.
        
		Returns the total size of the layout (i.e. the used amount of space that is covered by the layout).
        */
	Size calcLayoutImpl(ViewLayout* pLayout, const Size& availableSpace, bool forMeasuring) const;


	
};


}

#endif

