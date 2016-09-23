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

	

	Size	calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override;

	
protected:
	
	/** Calculates the positions and sizes (in DIPs - see UILength::Unit::dip) of the child views for the case that the ColumnView
		has the specified width.

		Returns the "useful" Size for the container contents (including padding and margins) in DIPs. Note that if the \c width parameter
        is bigger than the size needed to accomodate the widest child then the returned width will be smaller than the \c width
        parameter.
        
        pixelSizeDips is the size of 1 physical pixel in DIP units. This is usually <=1.
        */
	Size calcChildBoundsForWidth(double width, const std::list< P<View> >& childViews, std::list<Rect>& childBounds, double pixelSizeDips) const;

	void	layout() override;
};


}

#endif