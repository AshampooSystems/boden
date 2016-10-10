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

        forceLeftAlignment can be used to force all children to be left-aligned, ignoring their actual alignment values.
        This should be set to true if you want to determine the minimum useful width, without artificially enlarging or moving children
        to use all the available width.

		Returns the "useful" Size for the container contents (including padding and margins) in DIPs. Note that if the \c availableWidth parameter
        is bigger than the size needed to accomodate the widest child then the returned width will be smaller than the \c availableWidth
        parameter.        
        */
	Size calcChildBoundsForWidth(double availableWidth, const std::list< P<View> >& childViews, std::list<Rect>& childBoundsList, bool forceLeftAlignment) const;


	void	layout() override;
};


}

#endif

