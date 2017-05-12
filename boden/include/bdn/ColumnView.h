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

	

	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override;

	
protected:
	
	/** Calculates the positions and sizes (in DIPs - see UILength::Unit::dip) of the child views for the case that the ColumnView
		has the specified width.

        forMeasuring indicates that the call is not intended for arranging the children, but to measure the preferred size
        of the container. If this is true then it forces all children to be left-aligned, ignoring their actual alignment values.
        It also influences how the child bounds are rounded to full pixels.

        availableSpace indicates the amount of space that is available for the client views inside the parent.
        width and/or height of availableSpace can be Size::componentNone(), which means that the space is unlimited - i.e.
        the bounds should be measured for the case when any preferred size is acceptable.
        
		Returns the "useful" Size for the container contents (including padding and margins) in DIPs. Note that if the \c availableWidth parameter
        is bigger than the size needed to accomodate the widest child then the returned width will be smaller than the \c availableWidth
        parameter.        
        */
	Size calcChildBounds(const Size& availableSpace, const std::list< P<View> >& childViews, std::list<Rect>& childBoundsList, bool forMeasuring) const;


	void	layout() override;
};


}

#endif

